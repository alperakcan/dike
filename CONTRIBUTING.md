
# Contributing to Dike

Thank you for your interest in contributing to Dike! This document provides guidelines and information for contributors.

## Ways to Contribute

- **Bug Reports**: Report issues you encounter
- **Feature Requests**: Suggest new features or improvements
- **Code Contributions**: Submit bug fixes or new features
- **Documentation**: Improve or expand documentation
- **Testing**: Help test on different platforms and configurations

## Getting Started

### Prerequisites

- C++ compiler (GCC, Clang, or MSVC)
- Make
- Git with submodules support
- For WebAssembly builds: Emscripten SDK

### Building the Project

```bash
# Clone with submodules
git clone --recurse-submodules https://github.com/alperakcan/dike.git
cd dike

# Build native version
make

# Build WebAssembly version (requires emsdk)
emmake make __EMSDK__=y CMAKE="emcmake cmake" CXX=em++
```

## Contribution Workflow

1. **Fork the Repository**
   - Create a fork of the repository on GitHub
   - Clone your fork locally

2. **Create a Branch**
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/issue-description
   ```

3. **Make Your Changes**
   - Write clear, readable code
   - Follow the existing code style
   - Add comments for complex logic
   - Ensure double precision for coordinate calculations

4. **Test Your Changes**
   - Build and test both native and WebAssembly versions
   - Test with sample GPS tracks (GPX, KML, KMZ files)
   - Verify calculations are accurate

5. **Commit Your Changes**
   ```bash
   git add .
   git commit -m "Brief description of your changes"
   ```
   - Use clear, descriptive commit messages
   - Reference issue numbers if applicable (e.g., "Fixes #123")

6. **Push to Your Fork**
   ```bash
   git push origin feature/your-feature-name
   ```

7. **Submit a Pull Request**
   - Open a pull request from your fork to the main repository
   - Provide a clear description of your changes
   - Reference any related issues

## Code Style Guidelines

- Use consistent indentation (tabs as shown in existing code)
- Use meaningful variable and function names
- Keep functions focused and reasonably sized
- Add error checking and handling
- Use `double` for all floating-point coordinate calculations
- Avoid `float` to maintain precision in geospatial calculations

## Coordinate Precision Guidelines

When working with GPS coordinates and projections:

- Always use `double` (64-bit) for lat/lon and projected coordinates
- Use `std::sin`, `std::cos`, `std::atan`, etc. (not float variants)
- Write constants as `1.0` not `1.0f`
- Maintain consistency between native and WebAssembly builds
- Test coordinate transformations for accuracy

## Testing

- Test with various GPS file formats (GPX, KML, KMZ)
- Verify calculations with known test routes
- Check edge cases (international date line, poles, zone boundaries)
- Test in both native and WebAssembly environments
- Ensure browser compatibility for HTML/WASM versions

## Documentation

- Update README.md if adding new features
- Add inline comments for complex algorithms
- Update command-line help text if adding options
- Document any new dependencies

## License

By contributing to Dike, you agree that your contributions will be released into the public domain under the Unlicense. This means:

- Your contributions will be freely available to everyone
- No copyright restrictions will apply
- Attribution is appreciated but not required

See the [LICENSE](LICENSE) file for full details.

## Third-Party Dependencies

If your contribution adds a new dependency:

- Ensure the license is compatible (MIT, BSD, Boost, Apache 2.0, or public domain)
- Update the LICENSE file with attribution
- Document the dependency in README.md

## Questions or Help?

- Open an issue for questions
- Check existing issues and pull requests
- Review the README.md for project documentation

## Code of Conduct

- Be respectful and constructive
- Welcome newcomers and help them learn
- Focus on the technical merits of contributions
- Maintain a positive, collaborative environment

Thank you for contributing to Dike!
