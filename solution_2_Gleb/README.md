# Решение задачи "2. Верификация функции проверки прав доступа на Coq"

## Описание решения, основные идеи:

- Решение проверялось с помощью Coq 8.18.0.
- Использовался диалект Coq SSReflect, поэтому дополнительно импортировались модули ssreflect, ssrbool и eqtype библиотеки mathcomp.
- Также для проверки эквивалентности спецификаций импортировался модуль Zbitwise (для тактики Z.bitwise).

В ходе выполнения задачи объявлены две функции:
1. `inode_permission'` -- подробная спецификация функции `vsm_inode_permission`, максимально возможно соответствующая её коду.
2. `inode_permission` -- более удобная для использования в доказательствах спецификация той же функции.

Эквивалентность функций `inode_permission` и `inode_permission'` доказывается в конце файла -- безымянная теорема `Goal forall t i m, inode_permission' t i m = inode_permission t i m.`

Также для удобства доказывается промежуточная лемма lelP.

Спасибо за интересную задачу!