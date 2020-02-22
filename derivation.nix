{ stdenv
, lib
, pkgs
, cmake
, ninja
, boost
, qt5
, mesa
, mesa_drivers
, libGLU
}:
  
let
  stdenvClang              = pkgs.clangStdenv;
in
  stdenvClang.mkDerivation {
    name                   = "automated-tending";
    version                = "1.0";
    src                    = builtins.filterSource (p: t: lib.cleanSourceFilter p t && baseNameOf p != "build") ./.;

    nativeBuildInputs      = [
      cmake
      ninja
    ];

    propagatedBuildInputs  = [
      mesa
      mesa_drivers
      libGLU
    ];
  
    buildInputs            = [
      boost
      qt5.full
    ];

    cmakeFlags             = [
      "-DCMAKE_BUILD_TYPE=Release"
      "-GNinja"
    ];

    preConfigure           = ''
      cmakeFlags="$cmakeFlags"
    '';

    preBuild               = ''
      ninja clean
    '';

    buildPhase             = ''
      ninja
    '';

    makeTarget             = "automated-tending";
    enableParallelBuilding = true;

    doCheck                = false;
    checkTarget            = "test";

    installPhase           = ''
      mkdir -p $out/bin $out/bin/logs $out/bin/external $out/nix-support;
      cp -r conf external main $out/bin/;
    '';
  }
