Zadanie 1: Philosophers and Pipes
=================================

W systemie działa `N` procesów-filozofów ponumerowanych od 0 do `N-1` oraz `N` pipe'ów (ponumerowanych analogicznie).
Filozof o numerze `k` ma otwarty pipe numer `k` na deskryptorze 3 oraz pipe numer `(k+1)%N` na deskryptorze 4.
Są to końce pipe'ów służące do czytania.

Ponadto w systemie działa *Producent*, który w kolejnych fazach zapisuje do tych pipe'ów paczki z danymi.
W każdej fazie do każdego pipe'a trafia jedna paczka sładająca się z 2-byte'owego numeru fazy oraz 4-byte'owej liczby.
Liczba faz jest skończona.
Po zakończeniu pracy Producent pozamyka swoje końce ww pipe'ów.

Zadaniem filozofów jest konstruowanie *poprawnych paczek* z danymi i wypisywanie ich (binarnie) na standardowe wyjście.
Poprawna paczka ma rozmiar 10 byte'ów i składa się z jednej liczby 2 byte'owej `f` oraz dwóch liczb 4-byte'owch `x` oraz `y`.
Paczka jest poprawna jeśli:

- liczby `x` oraz `y` zostały wyprodukowane przez Producenta w tej samej fazie,
- liczba `x` została w tej fazie zapisana do pipe'a `f`,
- liczba `y` została w tej fazie zapisana do pipe'a `(f+1)%N`.

Jeśli wszystkie liczby wygenerowane przez Producenta są różne to każda z nich powinna trafić do co najwyżej jednej paczki.
W przypadku gdy mamy `N` filozofów oraz `r` faz, filozofowie powinni w sumie wyprodukować `r * floor(N/2)` poprawnych paczek.
Należy przy tym zadbać aby każdy z filozofów wyprodukował mniej więcej tyle samo (poprawnych) paczek.
Dopuszczalna różnica wynosi 1.


Zadanie polega na implementacji w języku C programu, który zostanie uruchomiony w procesach filozofów.
Program ten w pierwszym argumencie otrzyma liczbę wszystkich filozofów `N` a w drugim swój własny numer `k`.
Każdy z procesów filozofów już od momentu powstania będzie miał zablokowany sygnał `SIGUSR1`.
Ponadto, Producent, zanim zacznie produkować dane, zapisze (binarnie) do każdego pipe'a `k` pid filozofa o numerze `(k+1)%N`.

Rozwiązanie będzie testowane w systemie Linux.
