# lilygo47_calendar

*Read this in other language: [English](README.md), [Polski](README.pl.md).*

Aplikacja do wyświetlania wpisów z kalendarza Google na LILYGO 4,7".
![lilygo47_calendar](/img/img1.jpg)

# Sprzęt
W projekcie zostało wykorzystane urzadzenie LILIGO z EPD o przekątnej 4,7".
[http://www.lilygo.cn/](http://www.lilygo.cn/prod_view.aspx?TypeId=50031&Id=1384&FId=t3:50031:3)

# Opis działania
Urządzenie cyklicznie łączy się z wifi, a następnie pobiera informacje o zaplanowanych wydarzeniach (z wykorzystaniem AppScripts) z naszego kalendarza. Po pobraniu danych zostają one wyświetlone, a samo urządzenie zostaje uśpione.

Prezentowane dane składają się z dwóch głównych elementów:
* Pasek stanu,
* Informacja o wydarzeniach

Pasek stanu zawiera informacje (od lewej) o:
-- Ilości zadań z które nie zostały wyświetlone (nie zmieściły się na wyświetlaczu)
-- Dacie oraz godzinie pobrania informacji
-- Poziomie naładowania baterii oraz napięciu zasilania

# Konfiguracja / Wdrożenie / Programowanie
### Skrypt Google
#### Dodawanei skryptu w AppScript
W celu dodania skryptu który będzie pobierał i przygotowywał dane z kalendarza należy przejść na stronę [https://script.google.com](https://script.google.com).
Następnie należy wybrać "Nowy projekt". W oknie które się pojawi należy wkleić kod z pliku `calendar_script.gc`.
We fragmencie kodu
```
  var _calendarName = '___CALENDAR_NAME___',
    _checkInRate = SIXTYDAYS;
```
należy podmienić tekst `___CALENDAR_NAME___` na nazwę naszego kalendarza.

Jeżeli w Google mamy zalożoną rodzinę [https://families.google.com/families](https://families.google.com/families) to możemy współdzielić kalendarz pomiędzy wszystkie osoby. W takim przypadku należy `___CALENDAR_NAME___` podmienić na `Familijne`. Gotowy fragment kodu powinien wygladać następująco:
```
  var _calendarName = 'Familijne',
    _checkInRate = SIXTYDAYS;
```

#### Wdrażanie skryptu
Gdy skrypt jest już gotowy, w prawym górnym rogu strony wybieramy `Wdróż->Nowe wdrożenie`. W oknie które się pojawi wybieramy typ jako `Aplikacja internetowa`. 
W `Opis` podajemy dowolną nazwę (lub pozostawiamy puste).
W `Wykonaj jako` wybieramy `Ja`, natomiast w `Kto ma dostęp` ustawiamy `Każda osoba z kontem Google`.

Po wdrożeniu pojawia się okienko z którego kopiujemy `Identyfikator wdrożenia` - będzie nam potrzebne w dalszym etapie.

### Przygotowanie urządzenia

Kod został napisany i skompilowany w Visual Studio Code
```
Version: 1.57.1
Commit: 507ce72a4466fbb27b715c3722558bb15afa9f48
Date: 2021-06-17T13:26:56.255Z
Electron: 12.0.7
Chrome: 89.0.4389.128
Node.js: 14.16.0
V8: 8.9.255.25-electron.0
OS: Linux x64 5.11.0-25-generic
```
z wykorzystaniem wtyczki PlatformIO IDE w wersji 2.3.2.

Konfiguracja po stronie urządzenia ogranicza sie do przygotowania pliku z konfiguracją. Przykładowy plik konfiguracyjny znajduje się w katalogu `include/config.h_example`. Należy zmienić jego nazwę na `config.h` oraz uzupełnić dane dotyczące sieci WiFi oraz identyfilator skryptu.

```
#define WIFI_SSID             "Your WiFi SSID"
#define WIFI_PASSWORD         "Your PASSWORD"

#define SCRIPT_ID             "YOUR_GOOGLE_SCRIPT_ID"
```

Po wykonaniu powyższych czynności można podłączyć urządzenie LILYGO do komputera i wgrać oprogramowanie.

### Priorytety zadań
Każdy wpis w kalendarzu można opatrzyć odpowiednim kolorem który stanowi priorytet. Im wyższy priorytet tym ciemniejsza kropka jest wyświetlana przy wpisie.

| Nazwa            | Wartość z AppScript (int) | Wartość do EPD (nazwa, hex) |
|------------------|---------------------------|-----------------------------|
| Pomidor          | 11                        | LightGrey (0xBB)            |
| Mandarynka       | 6                         | LightGrey (0xBB)            |
| Szałwia          | 2                         | LightGrey (0xBB)            |
| Paw              | 7                         | Grey (0x88)                 |
| Lawenda          | 1                         | Grey (0x88)                 |
| Grafit           | 8                         | Grey (0x88)                 |
| Flaming          | 4                         | DarkGrey (0x44)             |
| Banan            | 5                         | DarkGrey (0x44)             |
| Bazylia          | 10                        | DarkGrey (0x44)             |
| Jagoda           | 9                         | Black (0x00)                |
| Winogrona        | 3                         | Black (0x00)                |
| Kolor kalendarza |                           | Grey (0x88)                 |


# Obudowa
W projekcie została wykorzsytana obudowa wykonana przez [3DMath](https://www.thingiverse.com/3dmath/designs)
Link do obudowy: https://www.thingiverse.com/thing:4908721

# Licencja
MIT

