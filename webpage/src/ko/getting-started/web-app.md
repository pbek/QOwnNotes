---
image: /img/blog/web-application-settings.png
---

# QOwnNotes 웹 앱

[app.qownnotes.org](https://app.qownnotes.org/)의 **웹 응용 프로그램**을 통해 전화기의 사진을 바탕 화면의 QOnNotes에 있는 현재 노트에 삽입할 수 있습니다.

![QOwnNotes 웹 응용프로그램 브라우저](/img/blog/web-application-browser.png "전화기에서 바탕 화면의 QOwnNotes로 사진 보내기")

전화기의 웹 응용프로그램은 인터넷을 통해 QOnNotes 데스크탑 응용프로그램과 통신합니다. QOnNotes와 웹 서비스 간의 통신은 **전송 암호화**되어 있습니다. 서버에 이미지가 저장되지 않습니다.

There is a settings page **Web application** to add support for the web application.

![QOownNotes 웹 응용프로그램 설정](/img/blog/web-application-settings.png "웹 응용 프로그램에 대한 통신 설정")

QOnNotes의 로컬 인스턴스와 통신할 수 있도록 QOnNotes 설정 페이지에서 [app.qownnotes.org](https://app.qownnotes.org/)로 **비밀 토큰을 복사**하는 것을 잊지 마십시오. 웹 애플리케이션에서 전화기로 QR 코드의 토큰을 스캔할 수도 있습니다.

웹 애플리케이션의 소스 코드는 [web-app on GitHub](https://github.com/qownnotes/web-app) web-app에서 확인할 수 있습니다.

또한 이 웹 서비스를 직접 호스팅할 수 있으며, 새로 구축된 도커 이미지는 [DockerHub의 qownnotes-web-app](https://hub.docker.com/repository/docker/pbeke/qownnotes-web-app)에서 이용할 수 있습니다.
