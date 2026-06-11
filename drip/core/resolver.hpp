#pragma once

#include <drip/core/package.hpp>
#include <vector>
#include <unordered_map>
#include <set>

namespace drip {

struct Literal {
    std::string package_name;
    Version selected_version;
    bool is_positive = true;
    size_t var_id = 0;

    bool operator==(const Literal&) const = default;
    auto operator<=>(const Literal& o) const {
        if (auto cmp = package_name <=> o.package_name; cmp != 0) return cmp;
        return is_positive <=> o.is_positive;
    }
};

struct Clause {
    std::vector<Literal> literals;
    bool is_learned = false;
    size_t activity = 0;
};

struct ResolutionResult {
    std::vector<ResolvedPackage> selected;
    std::vector<std::string> conflicts;
    bool success = false;
};

class Lockfile {
public:
    static Lockfile from_resolution(const ResolutionResult& result);

    void write_to(const std::string& path) const;
    std::string to_toml() const;

private:
    std::vector<ResolvedPackage> packages_;
};

class DependencyResolver {
public:
    explicit DependencyResolver(std::vector<Package> available_packages);

    ResolutionResult resolve(const Package& root,
                            const std::vector<std::string>& constraints = {});

    void enable_conflict_learning(bool enable) { conflict_learning_ = enable; }

private:
    struct Assignment {
        enum class Value { UNASSIGNED, TRUE, FALSE };
        Value value = Value::UNASSIGNED;
        size_t decision_level = 0;
        Literal* implied_by = nullptr;
    };

    std::unordered_map<std::string, std::vector<Package>> packages_by_name_;
    std::vector<Clause> all_clauses_;
    std::vector<Assignment> assignments_;
    std::unordered_map<std::string, size_t> var_id_map_;

    size_t decision_level_ = 0;
    bool conflict_learning_ = true;

    void encode_dependencies(const Package& pkg);
    bool cdcl_solve();
    bool unit_propagate();
    Literal pick_branch_literal();
    Clause analyze_conflict(const Clause& conflict_clause);
    void backtrack(size_t level);

    std::pair<std::string, std::string> parse_constraint(const std::string& constraint);
    std::vector<std::string> get_conflict_explanation();
    std::vector<Literal> find_antecedent_clause(const Literal& lit);

    struct VSIDSState {
        std::unordered_map<std::string, double> scores;
        size_t conflicts_since_last_decay = 0;
        static constexpr double DECAY_FACTOR = 0.95;
        void bump(const std::string& var);
        void decay();
    } vsids_;

    double calculate_activity(const Package& pkg) const;
    bool version_heuristic(const Version& a, const Version& b);
    bool platform_matches(const Dependency& dep) const;
};

}
