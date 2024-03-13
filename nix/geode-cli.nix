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
  version = "2.12.0";

  src = fetchzip {
    url = "https://github.com/geode-sdk/cli/releases/download/v${version}/geode-cli-v${version}-linux.zip";
    hash = "sha256-CZZ9G5oSFcT3OL0n0kP7dRgOfzaVBklNb7V86irrtlo=";
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
