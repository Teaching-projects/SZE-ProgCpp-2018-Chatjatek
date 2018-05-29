#pragma once
#include <stdexcept>


struct bad_version_err : public std::logic_error {
	bad_version_err(std::string const &s) : logic_error(s) {}
};

struct run_chain_err : public std::logic_error {
	run_chain_err(std::string const &s) : logic_error(s) {}
};

struct bad_input_err : public std::logic_error {
	bad_input_err(std::string const &s) : logic_error(s) {}
};