all: pcopy

check: pcopy
	bats check.bats
clean:
	rm pcopy
