
#pragma once

#include <string>
#include <stdexcept>

class Version
{
  private:
    int _major;
    int _minor;
    int _build;
    std::string _publishDate;

  public:
    inline Version(int major, int minor, int build)
      :_major(major), _minor(minor), _build(build), _publishDate("N/A")
    { /*nothing*/ }

    inline Version(int major, int minor, int build,
             const std::string publishDate)
      :_major(major), _minor(minor), _build(build), _publishDate(publishDate)
    { /*nothing*/ }

    inline const int &Major() const
    { return _major; }

    inline const int &Minor() const
    { return _minor; }

    inline const int &Build() const
    { return _build; }

    inline const std::string &PublishDate() const
    { return _publishDate; }

    std::string ToString() const;
};

class VersionConflictException : public std::exception
{
  private:
    Version _current;
    Version _required;

  public:
    VersionConflictException(const Version &current, const Version &required)
      : _current(current), _required(required)
    { /*nothing*/ }

    inline const Version &CurrentVersion()
    { return _current; }

    inline const Version &RequiredVersion()
    { return _required; }

    virtual const char *what() const throw()
    { return "The current version does not match the required one"; }
};

class CurrentVersion
{
  private:
    static Version *_programVersion;
    static int _fileFormatVersion;

  private:
    CurrentVersion() = delete;
    CurrentVersion(const CurrentVersion &) = delete;
    void operator =(const CurrentVersion &) = delete;

  public:
    static Version &ProgramVersion()
    { return *_programVersion; }

    static const int &FileFormatVersion()
    {return _fileFormatVersion; }
};
