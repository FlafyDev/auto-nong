{
  stdenv,
  fetchzip,
  autoPatchelfHook,
  zlib,
  openssl,
  libgcc,
}:
stdenv.mkDerivation rec {
  pname = "geode-cli";
  version = "3.0.0-beta.1";

  src = fetchzip {
    url = "https://github.com/geode-sdk/cli/releases/download/v${version}/geode-cli-v${version}-linux.zip";
    hash = "sha256-AtLlhPrVYt0pdeg4JTOmLEOIjPFjlnJGuBIYmHy4KCw=";
  };

  nativeBuildInputs = [autoPatchelfHook];

  buildInputs = [
    zlib
    openssl
    libgcc
  ];

  installPhase = ''
    runHook preInstall

    mkdir -p $out/bin
    cp geode $out/bin
    chmod +x $out/bin/geode

    runHook postInstall
  '';
}
