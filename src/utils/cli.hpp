#pragma once

#include <filesystem>

class CommandLineArgs
{
public:
  CommandLineArgs() = default;
  CommandLineArgs(int& argc, char**& argv);

  const std::filesystem::path& output_path() const { return output_path_; }
  size_t num_particles() const { return num_particles_; }

private:
  size_t num_particles_;
  std::filesystem::path output_path_;
};