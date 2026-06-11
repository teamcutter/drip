#pragma once

#include <drip/core/result.hpp>
#include <drip/core/version.hpp>
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <optional>
#include <filesystem>
#include <unordered_map>

namespace drip {

    class ResolvedPackage;

    struct Dependency {
        std::string name;
        std::string version_constraint;
        bool optional = false;
        std::optional<std::string> platform;
    };

    struct RegistrySource {
        std::string url;
        std::string checksum;
    };

    struct GitSource {
        std::string url;
        std::string rev;
    };

    struct PathSource {
        std::filesystem::path location;
    };

    using PackageSource = std::variant<RegistrySource, GitSource, PathSource>;

    class Package {
    public:
        Package();
        explicit Package(std::string name, Version version);

        Package(Package&&) noexcept = default;
        Package& operator=(Package&&) noexcept = default;
        Package(const Package&) = delete;
        Package& operator=(const Package&) = delete;

        ~Package();

        class Builder;

        const std::string& name() const noexcept;
        const Version& version() const noexcept;
        const std::vector<Dependency>& dependencies() const noexcept;
        const std::unordered_map<std::string, std::string>& metadata() const noexcept;

        std::string content_hash() const;
        bool supports_compiler(std::string_view compiler_id) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pimpl_;
    };

    struct Package::Impl {
        std::string name_;
        Version version_;
        std::vector<Dependency> deps_;
        std::unordered_map<std::string, std::string> meta_;
        PackageSource source_;
        bool header_only_ = false;
    };

    class Package::Builder {
    public:
        Builder& name(std::string name) { name_ = std::move(name); return *this; }
        Builder& version(Version version) { version_ = std::move(version); return *this; }
        Builder& source(PackageSource source) { source_ = std::move(source); return *this; }
        Builder& dependency(Dependency dep) { deps_.push_back(std::move(dep)); return *this; }
        Builder& metadata(std::string key, std::string value) {
            meta_[std::move(key)] = std::move(value);
            return *this;
        }
        Builder& header_only(bool yes) { header_only_ = yes; return *this; }

        Result<Package, std::string> build() const;

    private:
        std::string name_;
        Version version_;
        PackageSource source_;
        std::vector<Dependency> deps_;
        std::unordered_map<std::string, std::string> meta_;
        bool header_only_ = false;
    };

    class ResolvedPackage {
    public:
        std::string name;
        Version version;
        std::string source_url;
        std::string content_hash;
        std::vector<std::string> dependencies;
        std::filesystem::path install_path;

        std::string to_toml() const;
        static std::optional<ResolvedPackage> from_toml(std::string_view toml);
    };

}
