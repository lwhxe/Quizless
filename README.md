# Quizless

---

Quizless is a Command Line tool to train yourself on the first 30 elements of the periodic table.

The tool is written in C/C++ using Visual Studio 2022, all menus, prompts and text visible to the user is in Swedish.

The tool has been tested and has proven to be useful, it might eventually become a tool where other quiz activities can be scripted by the user.

The original version was implemented using G++ supplied by Cygwin. However for Windows 10/11 compatibility this was changed.

## How do I use it?

- Use [arrow up] and [arrow down] when moving in menus.
- Press [ENTER] to confirm or move into a selected menu.
- When prompted to: "Press [ENTER] to continue...", press [ENTER] to continue.
- Use the Swedish(SWE) keyboard for correct input.
- Press [ENTER] to send your query.

## What does it do?

- It uses "conio.h" for all menus.
- It measures the levenshtein distance between your query and the correct answer.
- Query is limited to the Swedish characters in lowercase.
- Query is automatically filtered into correct format.
- ANSI ESC codes and Unicode characters are used.
- To allow Unicode to display properly: system("chcp 65001"); is called.

## Report Errors, Issues and Suggestions:

- Describe your issue(s), error(s), or suggestion(s) in sufficient detail. (Answer: What's missing?)
- Give information on how to recreate your scenario. (What buttons did you press?)
- If you have solution to the issue(s), describe or give your solution.
