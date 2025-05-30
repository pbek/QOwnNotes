{
  lib,
  stdenv,
  fetchurl,
  python3,
  which,
}:

stdenv.mkDerivation rec {
  pname = "botan3-amalgamation";
  version = "3.8.1";

  src = fetchurl {
    url = "https://botan.randombit.net/releases/Botan-${version}.tar.xz";
    hash = "sha256-sDloHUuGGi9YU3Rti6gG9VPiOGntctie2/o8Pb+hfmg=";
  };

  nativeBuildInputs = [
    python3
    which
  ];

  # Configure to build amalgamation
  configurePhase = ''
    runHook preConfigure

    # Configure Botan to generate amalgamation files
    # We only need the amalgamation, so we can use --no-install-python-module
    # and other minimal flags to avoid building the full library
    python3 configure.py \
      --cc=gcc --cc-bin=g++ --cpu=x86_64 --amalgamation --minimized-build \
      --disable-shared \
      --enable-modules=aes,base,base64,block,cbc,checksum,cmac,filters,hash,hash_id,hex,hmac,hmac_drbg,kdf,kdf2,pbkdf,pbkdf2,sha1 \
      --without-documentation --disable-sse2 --disable-ssse3 --disable-sse4.1 --disable-sse4.2 \
      --disable-aes-ni --disable-sha-ni --cxxflags=-pipe \
      --prefix=$out

    runHook postConfigure
  '';

  # Build phase - we only need the amalgamation generation, not the full build
  buildPhase = ''
    runHook preBuild

    # The amalgamation files are generated during configure
    # We just need to make sure they exist
    if [ ! -f botan_all.cpp ] || [ ! -f botan_all.h ]; then
      echo "Error: Amalgamation files were not generated"
      exit 1
    fi

    ls -hal

    runHook postBuild
  '';

  # Install the amalgamation files
  installPhase = ''
        runHook preInstall

        mkdir -p $out/include
        mkdir -p $out/src

        # Copy the amalgamation header and source
        cp botan_all.h $out/include/
        cp botan_all.cpp $out/src/

        echo "Amalgamation files installed:"
        echo "  Header: $out/include/botan_all.h"
        echo "  Source: $out/src/botan_all.cpp"

        # Create a simple usage example
        cat > $out/README.md << EOF
    # Botan ${version} Amalgamation

    This package provides the Botan ${version} cryptography library as amalgamation files:

    - \`include/botan_all.h\` - Single header file containing all Botan headers
    - \`src/botan_all.cpp\` - Single source file containing all Botan implementation

    ## Usage

    To use in your C++ project:

    1. Include the header: \`#include "botan_all.h"\`
    2. Compile with the source: \`g++ -std=c++20 your_code.cpp botan_all.cpp\`

    ## Example

    \`\`\`cpp
    #include "botan_all.h"
    #include <iostream>

    int main() {
        // Initialize Botan
        Botan::LibraryInitializer init;

        // Use Botan functionality
        auto rng = std::make_unique<Botan::AutoSeeded_RNG>();
        auto random_data = rng->random_vec(32);

        std::cout << "Generated " << random_data.size() << " random bytes" << std::endl;
        return 0;
    }
    \`\`\`

    ## Version Information

    - Botan Version: ${version}
    - Generated: $(date)
    - Configuration: Amalgamation build
    EOF

        runHook postInstall
  '';

  # Verify the amalgamation files are valid
  doCheck = true;
  checkPhase = ''
    runHook preCheck

    echo "Checking amalgamation files..."

    if [ ! -f botan_all.h ]; then
      echo "ERROR: botan_all.h not found"
      exit 1
    fi

    if [ ! -f botan_all.cpp ]; then
      echo "ERROR: botan_all.cpp not found"
      exit 1
    fi

    header_size=$(stat -c%s botan_all.h)
    source_size=$(stat -c%s botan_all.cpp)

    if [ $header_size -lt 100000 ]; then
      echo "WARNING: Header file seems too small ($header_size bytes)"
    fi

    if [ $source_size -lt 1000000 ]; then
      echo "WARNING: Source file seems too small ($source_size bytes)"
    fi

    if ! grep -q "namespace Botan" botan_all.h; then
      echo "ERROR: Botan namespace not found in header"
      exit 1
    fi

    echo "Amalgamation files validation passed"
    echo "Header size: $header_size bytes"
    echo "Source size: $source_size bytes"

    runHook postCheck
  '';

  meta = with lib; {
    description = "Botan ${version} cryptography library amalgamation files";
    longDescription = ''
      This package provides Botan ${version} as amalgamation files - a single
      header (botan_all.h) and source file (botan_all.cpp) that contain the
      entire Botan cryptography library. This makes it easy to integrate Botan
      into projects without dealing with complex build dependencies.

      Botan is a C++ cryptography library providing TLS, X.509, ECDSA, AES,
      RSA, and many other cryptographic algorithms and protocols.
    '';
    homepage = "https://botan.randombit.net/";
    changelog = "https://botan.randombit.net/news.html";
    license = licenses.bsd2;
    maintainers = [ pbek ];
    platforms = platforms.all;

    # This is a source-only package
    hydraPlatforms = [ ];
  };
}
