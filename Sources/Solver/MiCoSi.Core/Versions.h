#pragma once
#include "Defs.h"

class Version
{
  public:
    Version(int major, int minor, int build)
      :_major(major), _minor(minor), _build(build), _publishDate("N/A")
    { /*nothing*/ }

    Version(int major, int minor, int build,
            const std::string publishDate)
      :_major(major), _minor(minor), _build(build), _publishDate(publishDate)
    { /*nothing*/ }

    Version(const Version &) = default;
    Version &operator =(const Version &) = default;

    const int Major() const
    { return _major; }

    const int Minor() const
    { return _minor; }

    const int Build() const
    { return _build; }

    const std::string &PublishDate() const
    { return _publishDate; }

    std::string ToString() const;

  private:
    int _major;
    int _minor;
    int _build;
    std::string _publishDate;
};


class VersionConflictException : public std::exception
{
  public:
    VersionConflictException() = delete;
    VersionConflictException(const Version &current, const Version &required)
      : _current(current), _required(required)
    { /*nothing*/ }
    VersionConflictException(const VersionConflictException &) = default;
    VersionConflictException &operator =(const VersionConflictException &) = default;

    const Version &CurrentVersion() const
    { return _current; }

    const Version &RequiredVersion() const
    { return _required; }

    virtual const char *what() const throw() override
    { return "The current version of MiCoSi does not match the required one"; }

  private:
    Version _current, _required;
};

class CompilationConflictException : public std::exception
{
  public:
    CompilationConflictException() = delete;
    CompilationConflictException(const std::string &current, const std::string &required)
      : _current(current), _required(required)
    { /*nothing*/ }
    CompilationConflictException(const CompilationConflictException &) = default;
    CompilationConflictException &operator =(const CompilationConflictException &) = delete;

    const std::string &CurrentFlags() const
    { return _current; }

    const std::string &RequiredFlags() const
    { return _required; }

    virtual const char *what() const throw() override
    { return "The current instance of MiCoSi was compiled with flags differ from the required ones"; }

  private:
    std::string _current, _required;
};


class CurrentVersion
{
  public:
    CurrentVersion() = delete;
    CurrentVersion(const CurrentVersion &) = delete;
    CurrentVersion &operator =(const CurrentVersion &) = delete;

    static const Version &ProgramVersion()
    { return *_programVersion; }

    static const int FileFormatVersion()
    { return _fileFormatVersion; }

    static std::string CompilationFlags();

  private:
    static Version *_programVersion;
    static int _fileFormatVersion;
};
