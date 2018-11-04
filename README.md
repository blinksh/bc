# `bc`

This is an implementation of POSIX `bc` that implements
[GNU `bc`](https://www.gnu.org/software/bc/) extensions, as well as the period
(`.`) extension for the BSD flavor of `bc`.

This `bc` also includes an implementation of `dc` in the same binary, accessible
via a symbolic link, which implements all FreeBSD and GNU extensions. If a
single `dc` binary is desired, `bc` can be copied and renamed to `dc`. The `!`
command is omitted; I believe this is poses security concerns and that such
functionality is unnecessary.

This `bc` is Free and Open Source Software (FOSS). It is offered under the BSD
0-clause License. Full license text may be found in the `LICENSE.md` file.

This version of `bc` builds upon the [original version](https://github.com/gavinhoward/bc). Changes are minimal, just to make it run with [ios_system](https://github.com/holzschu/ios_system), and thus inside terminals on iOS. 

## Build

To build for iOS, type the following commands: 
```
sh ./get_frameworks.sh
make
```
The first will download the latest version of the `ios_system` framework and the associated header file, `ios_error.h`. The second will create the auxiliary source files  (in the `gen/` subdirectory)

Then open `bc_ios/bc_ios.xcodeproj` and hit Build. This will create the `bc_ios.framework`, which you can then link with your iOS applications. 

### Optimization 

I ***highly*** encourage package and distro maintainers to compile as follows:

```
CPPFLAGS="-DNEBUG" CFLAGS="-O3" LDFLAGS="-s" make
```

The optimizations speed up `bc` by orders of magnitude. In addition, for SSE4
architectures, the following can add a bit more speed:

```
CPPFLAGS="-DNEBUG" CFLAGS="-O3 -march=native -msse4" LDFLAGS="-s" make
```

## Status

This `bc` is robust.

It is well-tested, fuzzed, and fully standards-compliant (though not certified)
with POSIX `bc`. The math has been tested with 30+ million random problems, so
it is as correct as I can make it.

This `bc` can be used as a drop-in replacement for any existing `bc`, except for
pass-by-reference array values. To build `bc` from source, an environment which
accepts GNU Makefiles is required.

The community is free to contribute patches for POSIX Makefile builds. This `bc`
is also compatible with MinGW toolchains.

It is also possible to download pre-compiled binaries for a wide list of
platforms, including Linux- and Windows-based systems, from
[xstatic](https://xstatic.musl.cc/bc/). This link always points to the latest
release of `bc`.

### Performance

This `bc` has similar performance to GNU `bc`. It is slightly slower on certain
operations and slightly faster on others. Full benchmark data are not yet
available.

#### Algorithms

This `bc` uses the math algorithms below:

##### Addition

This `bc` uses brute force addition, which is linear (`O(n)`) in the number of
digits.

##### Subtraction

This `bc` uses brute force subtraction, which is linear (`O(n)`) in the number
of digits.

##### Multiplication

This `bc` uses two algorithms:
[Karatsuba](https://en.wikipedia.org/wiki/Karatsuba_algorithm) and brute force.

Karatsuba is used for "large" numbers. ("Large" numbers are defined as any
number with `BC_NUM_KARATSUBA_LEN` digits or larger. `BC_NUM_KARATSUBA_LEN` has
a sane default, but may be configured by the user). Karatsuba, as implemented in
this `bc`, is superlinear but subpolynomial (bound by `O(n^log_2(3))`).

Brute force multiplication is used below `BC_NUM_KARATSUBA_LEN` digits. It is
polynomial (`O(n^2)`), but since Karatsuba requires both more intermediate
values (which translate to memory allocations) and a few more additions, there
is a "break even" point in the number of digits where brute force multiplication
is faster than Karatsuba. There is a script (`$ROOT/karatsuba.py`) that will
find the break even point on a particular machine.

***WARNING: The Karatsuba script requires Python 3.***

##### Division

This `bc` uses Algorithm D
([long division](https://en.wikipedia.org/wiki/Long_division)). Long division is
polynomial (`O(n^2)`), but unlike Karatsuba, any division "divide and conquer"
algorithm reaches its "break even" point with significantly larger numbers.
"Fast" algorithms become less attractive with division as this operation
typically reduces the problem size.

While the implementation of long division may appear to use the subtractive
chunking method, it only uses subtraction to find a quotient digit. It avoids
unnecessary work by aligning digits prior to performing subtraction.

Subtraction was used instead of multiplication for two reasons:

1.	Division and subtraction can share code (one of the goals of this `bc` is
	small code).
2.	It minimizes algorithmic complexity.

Using multiplication would make division have the even worse algorithmic
complexity of `O(n^(2*log_2(3)))` (best case) and `O(n^3)` (worst case).

##### Power

This `bc` implements
[Exponentiation by Squaring](https://en.wikipedia.org/wiki/Exponentiation_by_squaring),
and (via Karatsuba) has a complexity of `O((n*log(n))^log_2(3))` which is
favorable to the `O((n*log(n))^2)` without Karatsuba.

##### Square Root

This `bc` implements the fast algorithm
[Newton's Method](https://en.wikipedia.org/wiki/Newton%27s_method#Square_root_of_a_number)
(also known as the Newton-Raphson Method, or the
[Babylonian Method](https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Babylonian_method))
to perform the square root operation. Its complexity is `O(log(n)*n^2)` as it
requires one division per iteration.

##### Sine and Cosine

This `bc` uses the series

```
x - x^3/3! + x^5/5! - x^7/7! + ...
```

to calculate `sin(x)` and `cos(x)`. It also uses the relation

```
cos(x) = sin(x + pi/2)
```

to calculate `cos(x)`. It has a complexity of `O(n^3)`.

**Note**: this series has a tendency to *occasionally* produce an error of 1
[ULP](https://en.wikipedia.org/wiki/Unit_in_the_last_place). (It is an
unfortunate side effect of the algorithm, and there isn't any way around it;
[this article](https://people.eecs.berkeley.edu/~wkahan/LOG10HAF.TXT) explains
why calculating sine and cosine, and the other transcendental functions below,
within less than 1 ULP is nearly impossible and unnecessary.) Therefore, I
recommend that users do their calculations with the precision (`scale`) set to
at least 1 greater than is needed.

##### Exponentiation (Power of `e`)

This `bc` uses the series

```
1 + x + x^2/2! + x^3/3! + ...
```

to calculate `e^x`. Since this only works when `x` is small, it uses

```
e^x = (e^(x/2))^2
```

to reduce `x`. It has a complexity of `O(n^3)`.

**Note**: this series can also produce errors of 1 ULP, so I recommend users do
their calculations with the precision (`scale`) set to at least 1 greater than
is needed.

##### Natural Log

This `bc` uses the series

```
a + a^3/3 + a^5/5 + ...
```

(where `a` is equal to `(x - 1)/(x + 1)`) to calculate `ln(x)` when `x` is small
and uses the relation

```
ln(x^2) = 2 * ln(x)
```

to sufficiently reduce `x`. It has a complexity of `O(n^3)`.

**Note**: this series can also produce errors of 1 ULP, so I recommend users do
their calculations with the precision (`scale`) set to at least 1 greater than
is needed.

##### Arctangent

This `bc` uses the series

```
x - x^3/3 + x^5/5 - x^7/7 + ...
```

to calculate `atan(x)` for small `x` and the relation

```
atan(x) = atan(c) + atan((x - c)/(1 + x * c))
```

to reduce `x` to small enough. It has a complexity of `O(n^3)`.

**Note**: this series can also produce errors of 1 ULP, so I recommend users do
their calculations with the precision (`scale`) set to at least 1 greater than
is needed.

##### Bessel

This `bc` uses the series

```
x^n/(2^n * n!) * (1 - x^2 * 2 * 1! * (n + 1)) + x^4/(2^4 * 2! * (n + 1) * (n + 2)) - ...
```

to calculate the bessel function (integer order only).

It also uses the relation

```
j(-n,x) = (-1)^n * j(n,x)
```

to calculate the bessel when `x < 0`, It has a complexity of `O(n^3)`.

**Note**: this series can also produce errors of 1 ULP, so I recommend users do
their calculations with the precision (`scale`) set to at least 1 greater than
is needed.

##### Modular Exponentiation (`dc` Only)

This `dc` uses the
[Memory-efficient method](https://en.wikipedia.org/wiki/Modular_exponentiation#Memory-efficient_method)
to compute modular exponentiation. The complexity is `O(e*n^2)`, which may
initially seem inefficient, but `n` is kept small by maintaining small numbers.
In practice, it is extremely fast.

## Language

This `bc` is written in pure ISO C99.

## Commit Messages

This `bc` uses the commit message guidelines laid out in
[this blog post](http://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html).

## Semantic Versioning

This `bc` uses [semantic versioning](http://semver.org/).

## Contents

Files:

	.clang-format    Clang-format file, used only for cutting a release for busybox.
	install.sh       Install script.
	karatsuba.py     Script for package maintainers to find the optimal Karatsuba number.
	LICENSE.md       A Markdown form of the BSD 0-clause License.
	link.sh          A script to link dc to bc.
	Makefile         The Makefile.
	NOTICE.md        List of contributors and copyright owners.
	RELEASE.md       A checklist for making a release.
	release.sh       A script to run during the release process.
	safe-install.sh  Safe install script from musl libc.

Folders:

	dist     Files to cut toybox/busybox releases (maintainer use only).
	gen      The `bc` math library, help texts, and code to generate C source.
	include  All header files.
	src      All source code.
	tests    All tests.

