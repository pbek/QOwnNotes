# Nix를 통해 설치

[NixOS](https://nixos.org/) 및 기타 플랫폼에서 [Nix 패키지 관리자](https://nixos.wiki/wiki/Nix_package_manager)와 함께 QOnNotes를 설치할 수 있으며 패키지 이름은 [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes)입니다.

See [QOwnNotes on nix](https://search.nixos.org/packages?channel=unstable&show=qownnotes) for more information.

그런 다음 `qownnotes` 명령을 사용하여 QOnNotes를 실행할 수 있습니다.

::: 팁
QOwnNotes 명령줄 스니펫 관리자 [qc](https://github.com/qownnotes/qc)는 [Nix Store](https://search.nixos.org/packages?channel=unstable&show=qc)를 통해 설치할 수도 있습니다

`nix-shell -p qc --run qc` 을 사용하여 사용해 볼 수 있습니다.
:::
