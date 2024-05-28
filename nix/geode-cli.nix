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
  version = "2.13.1";

  src = fetchzip {
    url = "https://github.com/geode-sdk/cli/releases/download/v${version}/geode-cli-v${version}-linux.zip";
    hash = "sha256-2yT7x/m0V+is8k4R1ki/aqzCbyetBE6UXdr67/BnvDc=";
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
