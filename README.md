# FuzzyWatch


This is just a simple watchface to get myself acquainted with [CloudPebble](cloudpebble.com). The fuzzy watch will display the current time as (german) text. To translate just change `s_hours_text` and `s_fuzzy_text`.

This is the mapping I used for the indices of the fuzzy text:

idx | minute | +3 | length
---|---|---|---
0 | 57-3 | 0-6 | 7
1 | 4-11 | 7-14 | 8
2 | 12-18 | 15-21 | 7
3 | 19-26 | 22-29 | 8
4 | 27-33 | 30-36 | 7
5 | 34-41 | 37-44 | 8
6 | 42-48 | 45-51 | 7
7 | 49-56 | 52-59 | 8

Screenshot:

![pebble screenshot](https://raw.github.com/octomike/FuzzyWatch/master/screenshot.png)
