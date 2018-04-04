# spelchekr
**spelchekr** is a simple command-line tool that aids in spell-checking text files.
It computes a *Levenshtein distance matrix* for each word that does not match those in a predefined word list. Users are presented with 10 of the closest matches based on their edit distances.
Because of the inefficient nature of this algorithm, misspellings and their corrections are stored in individual files.
Superior options exist for spell-checking -- this tool and its source are provided here for demonstrative purposes only.
