#include <drip/core/package.hpp>

namespace drip {

Package::Package() : pimpl_(std::make_unique<Impl>()) {}

Package::Package(std::string name, Version version)
    : pimpl_(std::make_unique<Impl>())
{
    pimpl_->name_ = std::move(name);
    pimpl_->version_ = std::move(version);
}

Package::~Package() = default;

const std::string& Package::name() const noexcept { return pimpl_->name_; }
const Version& Package::version() const noexcept { return pimpl_->version_; }
const std::vector<Dependency>& Package::dependencies() const noexcept { return pimpl_->deps_; }
const std::unordered_map<std::string, std::string>& Package::metadata() const noexcept { return pimpl_->meta_; }

std::string Package::content_hash() const { return {}; }

bool Package::supports_compiler(std::string_view) const { return true; }

Result<Package> Package::Builder::build() const {
    if (name_.empty()) {
        return Result<Package>(Error{"Package name is required"});
    }
    Package pkg;
    pkg.pimpl_->name_ = name_;
    pkg.pimpl_->version_ = version_;
    pkg.pimpl_->deps_ = deps_;
    pkg.pimpl_->meta_ = meta_;
    pkg.pimpl_->source_ = source_;
    pkg.pimpl_->header_only_ = header_only_;
    return Result<Package>(std::move(pkg));
}

std::string ResolvedPackage::to_toml() const {
    std::string result;
    result += "name = \"" + name + "\"\n";
    result += "version = \"" + version.to_string() + "\"\n";

    if (!source_url.empty())
        result += "source_url = \"" + source_url + "\"\n";

    if (!content_hash.empty())
        result += "content_hash = \"" + content_hash + "\"\n";

    if (!dependencies.empty()) {
        result += "dependencies = [";
        for (size_t i = 0; i < dependencies.size(); i++) {
            if (i > 0) result += ", ";
            result += "\"" + dependencies[i] + "\"";
        }
        result += "]\n";
    }

    if (!install_path.empty())
        result += "install_path = \"" + install_path.string() + "\"\n";

    return result;
}

std::optional<ResolvedPackage> ResolvedPackage::from_toml(std::string_view) {
    return std::nullopt;
}

}
