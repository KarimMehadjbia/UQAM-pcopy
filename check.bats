#!/usr/bin/env bats

load test_helper

@test "base-output" {
	run ./pcopy tests/hello 12-6
	check 0 "6"
}

@test "base-copies" {
	run ./pcopy tests/hello 12-6
	run ls copies/hello
	check 0 "copies/hello"
}

@test "base-contenu" {
	run ./pcopy tests/hello 12-6
	run cat copies/hello
	check 0 "onde!"
}

@test "copies" {
	run mkdir copies
	run ./pcopy tests/hello 12-6
	check 0 "6"
}

@test "limit-usecase-1" {
	run ./pcopy tests/hello 12-0
	check 0 "0"
}

@test "limit-usecase-2" {
	run ./pcopy tests/hello 0-20
	check 0 "18"
}

@test "inter-output" {
	run ./pcopy tests/hello 12-6 tests/other/bonjour.java 50-43
	check 0 "49"
}

@test "inter-copies" {
	run ./pcopy tests/hello 12-6
	run ls copies
	checki 0 <<FIN
bonjour.java
hello
FIN
}

@test "inter-contenu-1" {
	run ./pcopy tests/hello 12-6 tests/other/bonjour.java 50-43
	run cat copies/hello
	check 0 "onde!"
}

@test "inter-contenu-2" {
	run ./pcopy tests/hello 12-6 tests/other/bonjour.java 50-43
	run cat copies/bonjour.java
	checki 0 <<FIN
rgs[]){
    System.out.println("Hello Java"
FIN
}
