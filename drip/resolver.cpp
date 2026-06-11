#include <drip/resolver.hpp>
#include <fstream>
#include <algorithm>

namespace drip {

Lockfile Lockfile::from_resolution(const ResolutionResult& result) {
    Lockfile lf;
    lf.packages_ = result.selected;
    return lf;
}

void Lockfile::write_to(const std::string& path) const {
    std::ofstream file(path);
    file << to_toml();
}

std::string Lockfile::to_toml() const {
    std::string result;
    for (const auto& pkg : packages_) {
        result += "[[packages]]\n";
        result += pkg.to_toml();
        result += "\n";
    }
    return result;
}

DependencyResolver::DependencyResolver(std::vector<Package> available_packages) {
    for (auto& pkg : available_packages) {
        packages_by_name_[pkg.name()].push_back(std::move(pkg));
    }
}

std::pair<std::string, std::string> DependencyResolver::parse_constraint(const std::string& constraint) {
    auto pos = constraint.find_first_of(">=<");
    if (pos == std::string::npos) {
        return {constraint, ""};
    }
    return {constraint.substr(0, pos), constraint.substr(pos)};
}

std::vector<std::string> DependencyResolver::get_conflict_explanation() {
    std::vector<std::string> explanations;
    for (auto& clause : all_clauses_) {
        if (clause.literals.empty()) {
            explanations.push_back("Unsatisfiable constraint");
        }
    }
    if (explanations.empty()) {
        explanations.push_back("No solution found");
    }
    return explanations;
}

std::vector<Literal> DependencyResolver::find_antecedent_clause(const Literal& lit) {
    for (auto& clause : all_clauses_) {
        for (auto& clit : clause.literals) {
            if (clit.package_name == lit.package_name && clit.var_id == lit.var_id) {
                std::vector<Literal> result;
                for (auto& alit : clause.literals) {
                    if (!(alit == lit)) {
                        result.push_back(alit);
                    }
                }
                return result;
            }
        }
    }
    return {};
}

void DependencyResolver::encode_dependencies(const Package& pkg) {
    Clause unit_clause;
    Literal lit;
    lit.package_name = pkg.name();
    lit.selected_version = pkg.version();
    lit.is_positive = true;
    unit_clause.literals.push_back(lit);
    all_clauses_.push_back(unit_clause);

    for (auto& dep : pkg.dependencies()) {
        Clause dep_clause;
        auto& versions = packages_by_name_[dep.name];
        for (auto& vpkg : versions) {
            Literal dlit;
            dlit.package_name = vpkg.name();
            dlit.selected_version = vpkg.version();
            dlit.is_positive = true;
            dep_clause.literals.push_back(dlit);
        }
        if (!dep_clause.literals.empty()) {
            all_clauses_.push_back(dep_clause);
        }
    }
}

bool DependencyResolver::unit_propagate() {
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto& clause : all_clauses_) {
            size_t unassigned_count = 0;
            Literal* last_unassigned = nullptr;
            bool clause_true = false;

            for (auto& lit : clause.literals) {
                auto& assignment = assignments_[lit.var_id];
                if (assignment.value == Assignment::Value::TRUE && lit.is_positive) {
                    clause_true = true;
                    break;
                } else if (assignment.value == Assignment::Value::FALSE && !lit.is_positive) {
                    clause_true = true;
                    break;
                } else if (assignment.value == Assignment::Value::UNASSIGNED) {
                    unassigned_count++;
                    last_unassigned = &lit;
                }
            }

            if (clause_true) continue;

            if (unassigned_count == 0) {
                if (conflict_learning_) {
                    Clause learned = analyze_conflict(clause);
                    all_clauses_.push_back(learned);
                }
                return false;
            }

            if (unassigned_count == 1) {
                Literal& forced_lit = *last_unassigned;
                assignments_[forced_lit.var_id].value =
                    forced_lit.is_positive ? Assignment::Value::TRUE : Assignment::Value::FALSE;
                assignments_[forced_lit.var_id].decision_level = decision_level_;
                assignments_[forced_lit.var_id].implied_by = &forced_lit;
                changed = true;
                vsids_.bump(forced_lit.package_name);
            }
        }
    }
    return true;
}

Literal DependencyResolver::pick_branch_literal() {
    for (size_t i = 0; i < assignments_.size(); i++) {
        if (assignments_[i].value == Assignment::Value::UNASSIGNED) {
            for (auto& [name, pkgs] : packages_by_name_) {
                for (auto& pkg : pkgs) {
                    if (var_id_map_[pkg.name()] == i) {
                        Literal lit;
                        lit.package_name = pkg.name();
                        lit.selected_version = pkg.version();
                        lit.is_positive = true;
                        lit.var_id = i;
                        return lit;
                    }
                }
            }
        }
    }
    return {};
}

void DependencyResolver::backtrack(size_t level) {
    for (auto& assign : assignments_) {
        if (assign.decision_level > level) {
            assign.value = Assignment::Value::UNASSIGNED;
            assign.decision_level = 0;
            assign.implied_by = nullptr;
        }
    }
    decision_level_ = level;
}

bool DependencyResolver::cdcl_solve() {
    while (true) {
        if (unit_propagate()) {
            bool all_assigned = true;
            for (auto& assign : assignments_) {
                if (assign.value == Assignment::Value::UNASSIGNED) {
                    all_assigned = false;
                    break;
                }
            }
            if (all_assigned) return true;

            decision_level_++;
            Literal branch = pick_branch_literal();
            assignments_[branch.var_id].value = Assignment::Value::TRUE;
            assignments_[branch.var_id].decision_level = decision_level_;
        } else {
            if (decision_level_ == 0) return false;
            backtrack(decision_level_ - 1);
        }
    }
}

Clause DependencyResolver::analyze_conflict(const Clause& conflict) {
    Clause learned;
    std::set<Literal> seen;
    std::vector<Literal> worklist = conflict.literals;

    while (!worklist.empty()) {
        Literal lit = worklist.back();
        worklist.pop_back();

        if (seen.contains(lit)) continue;
        seen.insert(lit);

        auto& assign = assignments_[lit.var_id];

        if (assign.decision_level == decision_level_ && assign.implied_by != nullptr) {
            for (const auto& antecedent_lit : find_antecedent_clause(lit)) {
                worklist.push_back(antecedent_lit);
            }
        } else if (assign.decision_level > 0) {
            learned.literals.push_back(lit);
        }
    }

    for (auto& lit : learned.literals) {
        lit.is_positive = !lit.is_positive;
    }

    learned.is_learned = true;
    return learned;
}

void DependencyResolver::VSIDSState::bump(const std::string& var) {
    scores[var] += 1.0;
    if (++conflicts_since_last_decay >= 100) {
        decay();
        conflicts_since_last_decay = 0;
    }
}

void DependencyResolver::VSIDSState::decay() {
    for (auto& [var, score] : scores) {
        score *= DECAY_FACTOR;
    }
}

ResolutionResult DependencyResolver::resolve(const Package& root,
                                             const std::vector<std::string>& constraints) {
    all_clauses_.clear();
    assignments_.clear();
    var_id_map_.clear();
    decision_level_ = 0;

    size_t var_id = 0;
    for (auto& [name, pkgs] : packages_by_name_) {
        for (auto& pkg : pkgs) {
            var_id_map_[pkg.name()] = var_id;
            var_id++;
        }
    }

    assignments_.resize(var_id);

    encode_dependencies(root);

    for (const auto& constraint : constraints) {
        auto [name, version_constraint] = parse_constraint(constraint);
        (void)name;
        (void)version_constraint;
    }

    if (!cdcl_solve()) {
        return ResolutionResult{.conflicts = get_conflict_explanation(), .success = false };
    }

    ResolutionResult result;
    result.success = true;
    for (const auto& [pkg_name, var] : var_id_map_) {
        if (assignments_[var].value == Assignment::Value::TRUE) {
            for (const auto& pkg : packages_by_name_[pkg_name]) {
                ResolvedPackage rp;
                rp.name = pkg.name();
                rp.version = pkg.version();
                rp.content_hash = pkg.content_hash();
                result.selected.push_back(rp);
                break;
            }
        }
    }

    return result;
}

double DependencyResolver::calculate_activity(const Package& pkg) const {
    return static_cast<double>(pkg.dependencies().size());
}

bool DependencyResolver::version_heuristic(const Version& a, const Version& b) {
    return a > b;
}

bool DependencyResolver::platform_matches(const Dependency& dep) const {
    return !dep.platform.has_value();
}

}
