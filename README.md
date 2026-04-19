# Reziduální aritmetika - Zápočťák na P2X

## Specifikace

K zápočtovému programu jsem si vybral téma Reziduální aritmetika, jehož
součástí je implementace provádění aritmetických operací s dlouhými čísly za
pomocí poznatků o zbytcích po dělení. Každé číslo v určitém rozmezí umíme
jednoznačně vyjádřit jako seznam zbytků po dělení několika nesoudělnými čísly a
s tímto seznamem pak velmi efektivně sčítat, odčítat a násobit.

Vytvořil jsem tedy knihovnu v jazyce C, která implementuje veškeré potřebné
operace, jako je převod z desítkového zápisu na zápis se zbytky a naopak a pak
operace sčítání, odčítání, násobení a porovnávání.

## Sestavení

K sestavení programu a dokumentace je potřeba:
  - `gcc`, `make`
  - `texlive`, `python`, `doxygen`

Příkaz `make` nabízí následující příkazy:
  - `make library` -- sestaví staticky linkovanou knihovnu do `build/libzapoctak.a`
  - `make sharedlibrary` -- sestaví dynamicky linkovanou knihovnu do `build/libzapoctak.so`
  - `make examples` -- sestaví příklady programů ve složce `examples` do `build/examples`
  - `make docs` -- sestaví jednu celkovou dokumentaci programu v souboru `docs/build/docs.pdf` a pak API dokumentaci vygenerovanou pomocí `doxygen` v `docs/build/html/index.html`
  - `make` a `make all` -- spustí všechny příkazy výše
  - `make clean` -- smaže kompilované soubory spojené s programem
  - `make cleandocs` -- smaže kompilované soubory spojené s dokumentací

Zbytek informací lze najít v podrobnější dokumentaci.
