# This file is part of "austin" which is released under GPL.
#
# See file LICENCE or go to http://www.gnu.org/licenses/ for full license
# details.
#
# Austin is a Python frame stack sampler for CPython.
#
# Copyright (c) 2019 Gabriele N. Tornetta <phoenix1987@gmail.com>.
# All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

load "common"


function invoke_austin {
  local version="${1}"

  check_python $version

  log "Fork Multi-processing [Python $version]"

  # -------------------------------------------------------------------------
  step "Profiling of multi-process program"
  # -------------------------------------------------------------------------
    run $AUSTIN -i 100ms -C $PYTHON test/target_mp.py

    assert_success

    expected=3
    n_procs=$( echo "$output" | sed -r 's/P([0-9]+);.+/\1/' | sort | uniq | wc -l )
    assert "At least 3 parallel processes" "$n_procs -ge $expected"

    assert_output "do (.*test/target_mp.py);L[[:digit:]]*;fact (.*test/target_mp.py);L"

}


# -----------------------------------------------------------------------------
# -- Test Cases
# -----------------------------------------------------------------------------

@test "Test Austin with Python 2.3" {
  skip "Multiprocessing library introduced in Python 2.6"
	repeat 3 invoke_austin "2.3"
}

@test "Test Austin with Python 2.4" {
  skip "Multiprocessing library introduced in Python 2.6"
	repeat 3 invoke_austin "2.4"
}

@test "Test Austin with Python 2.5" {
  skip "Multiprocessing library introduced in Python 2.6"
	repeat 3 invoke_austin "2.5"
}

@test "Test Austin with Python 2.6" {
	repeat 3 invoke_austin "2.6"
}

@test "Test Austin with Python 2.7" {
	repeat 3 invoke_austin "2.7"
}

@test "Test Austin with Python 3.3" {
	repeat 3 invoke_austin "3.3"
}

@test "Test Austin with Python 3.4" {
	repeat 3 invoke_austin "3.4"
}

@test "Test Austin with Python 3.5" {
	repeat 3 invoke_austin "3.5"
}

@test "Test Austin with Python 3.6" {
  repeat 3 invoke_austin "3.6"
}

@test "Test Austin with Python 3.7" {
  repeat 3 invoke_austin "3.7"
}

@test "Test Austin with Python 3.8" {
  repeat 3 invoke_austin "3.8"
}

@test "Test Austin with Python 3.9" {
  repeat 3 invoke_austin "3.9"
}
