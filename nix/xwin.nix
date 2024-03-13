{
  stdenv,
  fetchzip,
  autoPatchelfHook,
  zlib,
  openssl,
  libgcc,
}:
stdenv.mkDerivation rec {
  pname = "xwin";
  version = "0.5.0";

  src = fetchzip {
    url = "https://github.com/Jake-Shadle/xwin/releases/download/${version}/xwin-${version}-x86_64-unknown-linux-musl.tar.gz";
    hash = "sha256-/Iu8qQAIfmrQvYNofMZrdn1rk3vNAf8OI03dnaaRMxY=";
  };

  nativeBuildInputs = [autoPatchelfHook];

  buildInputs = [
    # zlib
    # openssl
    # libgcc
  ];

  installPhase = ''
    runHook preInstall

    mkdir -p $out/bin
    cp xwin $out/bin
    chmod +x $out/bin/xwin

    runHook postInstall
  '';
}

