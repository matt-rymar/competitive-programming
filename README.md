<!-- @format -->

# Competitive Programming Library/Solutions

## What's in the repo?

This repo contains code written for and in
[competitive programming](https://en.wikipedia.org/wiki/Competitive_programming)
settings. The vast majority of the codebase has been written in 2020-2021. It
has not been touched for a while, save for some clang-format runs before
uploading to GitHub. I'm mostly uploading it for historical reasons, but perhaps
someone will happen upon this repo and find it useful :)

If you do happen to use it, feel free to shoot me a message, I'd be curious to
know! I've got a [Codeforces](https://codeforces.com/profile/MattRymar) and a
[LinkedIn](https://www.linkedin.com/in/matt-rymar/) account.

## Why does the code look weird? Where are my unit tests?

This is not a typical C++ library. In fact, the files in this repo generally
don't even have headers. This is due to how competitive programming contests
work: generally speaking, solutions to problems have to be uploaded as a single
file. When using the libraries, the user therefore has to play the role of a
preprocessor and paste in the necessary files.

Additionally, competitive programming has a very unique set of constraints that
lead to the code being quite unlike other software:

- **Programming speed is crucial:** The contests are timed, so being able to
  type out a solution to a problem as fast as possible is vital. That one
  additional minute you've spent factoring out a function while solving the
  first problem of a contest? There go 100 places in the ranking.
- **The resulting code is extremely short lived:** once a solution passes tests,
  the code can basically be discarded. Hence, the code's lifespan is <1 hour and
  there are no considerations about maintainability to be made.

The two factors above lead to code which would otherwise be considered
bad/terrible practice. Examples include, but are not limited to:

- **Extreme abbreviations to speed up typing:** One letter/shortened variable
  names are common. Aliases for standard common library facitilies are
  introduced: "std::vector" becomes "V", "int64_t" becomes "i64", etc. Silly as
  it sounds, these really matter: I remember resisting those for quite some time
  as they (imo) hinder readability quite a bit, but was eventually forced to
  cave in because of how much time they save in a contest setting.
- **No unit tests coverage for library functions:** The library functions were
  generally developed when a need arose. Hence, the problems they were used in
  served as unit test suites. That being said, I'd love to get back to the
  library at some point and add proper unit test coverage--as is, refactoring is
  difficult as most of the library functions don't have nice links to past
  problems that can be used as tests after refactoring. Additionally, some
  algorithms may have weird edge cases that may not be hit by all problems using
  it.
- **Little abstraction:** It is not uncommon for the core logic of a problem
  solution to be all stuffed into the `main()` method of the program.
- **Unidiomatic usage of the language:** C-style casts cause `int`s turned out
  to be inconvient? Yep. Everything is a `std::pair` cause you didn't have the
  time to define a struct instead? Of course!

## What makes this repo nice?

The library code had been developed with two principles in mind:

- **As fast as necessary, but no more:** the code has to be fast enough to pass
  tests on online judges. However, the constraints are usually quite permissive
  when used with an efficient language like C++, so a solution will generally
  pass as long as it has the right asymptotic complexity. This means that if a
  tradeoff between readability and speed exists, we go with the former. The
  downside of this approach is that it happens less often that we can get away
  with a solution of the wrong asymptotic complexity that is just fast enough to
  barely pass.
- **As readable as possible:** While we might have to ignore some best
  practices, the library code should still be somewhat readable. A person
  familiar with the ideas behind the algorithms/data structures should be able
  to follow what the code does.

While some corners had been cut here or there (looking through it while
uploading the repo to GitHub, I see a lot of code I'd love to refactor to
reflect more of the best practices), the end result should still be a good
library for competitve programming.

## What's less nice?

The library is incomplete. It covers the basics, enough for most Codeforces Div.
2 contests. However, some algorithms useful for competitive programming are
missing. Examples include:

- Max Flow, Minimum Cost Maximum Flow (MCMF).
- Linear Programming (LP), Simplex algorithm.
- Gaussian elimination and other linear algebra functions.
- String hashing, e.g., a rolling hash.
- Heavy-Light Decomposition.
- Sqrt Decomposition.

... and many more.

## Structure

The repo has two main parts: library and solutions.

- **library:** generic code to be used across problems. `Template.cpp` contains
  the core driver code and the remaining libraries can be pasted as needed.
- **solutions:** those are solutions to past problems from across different
  competitive programming websites.

## How to use

If you'd like to use the library part of the repo, the typical usage would be:

- Use `Template.cpp` as the starting point of your solution.
- Copy-paste other libraries as needed into the solution. Note that some
  libraries rely on functions/classes from other libraries, including the
  template. Hence, don't be surprised to get a compilation error and add the
  dependencies as needed.
