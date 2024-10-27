/*
* Функция compute_mask вычисляет маску (с битами MAY_WRITE и MAY_READ), которая в дальнейшем используется для
* принятия решения о доступе к заданному файлу. 
* Отметим, что доступы в маске означают наличие прав соответствующего доступа

* Функция check_permission проверяет доступ к заданному файлу с учетом прав пользователя на чтение/запись.
* Функция возвращает 3 значения:
* 0  – доступ к файлу разрешен;
* -13 – доступ к файлу запрещен (макрос NO_PERM); 
* -1 – недостаточно высокий уровень целостности пользователя, ошибка доступа (макрос NO_ILEV).

* Также используется выражение (MAY_READ | MAY_WRITE), разрешающее и чтение, и запись для указанного файла.

* ВНИМАНИЕ! 
* В коде одной из функций есть ошибка. Найдите её при помощи верификации, не изменяя уже написанные спецификации к коду.
* Исправьте код (обосновав своё решение) и полностью проверифицируйте получившиеся функции (все цели должны быть доказаны).

*/

// Определяем макросы
#define NO_PERM 13
#define NO_ILEV 1
#define NO_LABEL 12
#define MAY_WRITE 0x00000002
#define MAY_READ 0x00000004

#define IMPORTANT 50000
#define REGULAR 0x5414
#define ORDINARY 0x5413
#define RARE 0x541B
#define EXOTIC 2
#define SHIFT (1 << 9)

// Структура файла содержит 2 поля:
struct file {
    struct inode *f_inode; // поле индексного дескриптора и 
    int f_mode; // поле режима обработки
};

// Структура индексного дескриптора содержит 2 поля:
struct inode {
    unsigned int i_mode; // поле режима обработки и 
    unsigned int i_flags; // поле служебных флагов 
};

// Структура метки процесса,
// которая содержит следующие поля:
typedef struct {
    unsigned int lev; // поле иерархического уровня доступа;
    unsigned int ilev; // поле уровня целостности пользователя/процесса;
    unsigned int cat; // поле категорий целостности пользователя/процесса
} PDPL_T;

// Структура процесса содержит следующие поля:
struct process {
    unsigned int usage; // поле счетчика использования;
    unsigned int fsuid; // поле, определяющее, является ли процесс суперпользовательским (корневым);
    void *security; // поле структуры безопасности
};

// Структура текущей задачи содержит следующие поля:
struct task {
    unsigned int usage; // поле счетчика использования;
    const struct process *process; // поле структуры процесса;
    const PDPL_T *l; // поле метки процесса
};

// Глобальная переменная текущей задачи/процесса
struct task *current;

// Структура, сопоставляющая типу события определенное право доступа
static const unsigned int event_numbers[][2] = {
    { REGULAR, MAY_WRITE },
    { ORDINARY, MAY_READ },
    { RARE, MAY_READ },
};

// Глобальная переменная максимального уровня целостности
unsigned int max_ilev = 123400000;

// Аксиоматика; можете дополнять своими определениями 
/*@ axiomatic Block {
        logic unsigned int integrityCategories(PDPL_T *l) = l->ilev;
        
        predicate currentIsFileSystemRoot =
            current->process->fsuid == 0;
        
        logic PDPL_T *getSecurityLabel(struct task *task) =
            task->l;

        predicate isPublicFile(struct file *f) =
            f->f_inode->i_flags & SHIFT;
        
        predicate isGTImportant(unsigned int cmd) =
            cmd > IMPORTANT;

        predicate isLTExotic(unsigned int cmd) =
            cmd < EXOTIC;

        predicate isMaxILev(struct task *task) =
            getSecurityLabel(task)->ilev == max_ilev;

        predicate valid_result_check_permission(int result) =
            result == 0 || result == -NO_PERM || result == -NO_ILEV || result == -NO_LABEL;

        predicate valid_cmd(unsigned int cmd) = 
            cmd >= 0 && cmd <= 0xFFFFFFFF;
                       
        predicate valid_mode(unsigned int mode) =
            mode >= 0 && mode <= 0xFFFFFFFF;

        predicate valid_flags(unsigned int flags) = 
            flags >= 0 && flags <= 0xFFFFFFFF;

        predicate valid_inode(struct inode *i) =
            \valid(i) && valid_mode(i->i_mode) && valid_flags(i->i_flags);

        predicate valid_file(struct file *f) =
            \valid(f) && \valid(f->f_inode) && valid_inode(f->f_inode) && f->f_mode >= 0;
    }
*/

// Функция-геттер, возвращает поле индексного дескриптора
/*@ requires \valid(f);
    assigns \result \from f->f_inode;
    ensures \result == f->f_inode;
*/
static inline struct inode *file_inode(const struct file *f)
{
    return f->f_inode;
}

// Функция-геттер, возвращает поле уровня целостности пользователя/процесса;
/*@ requires \valid(sl);
    assigns \nothing;
    ensures \result == sl->ilev;
*/
unsigned int slabel_ilev(const PDPL_T *sl)
{
   return sl->ilev;
}

// Функция-геттер, возвращает метку безопасности пользователя/процесса;
/*@ requires \valid(current);
    requires \valid(current->l) || current->l == \null;
    assigns \result \from current->l;
    ensures \result == current->l;
*/
PDPL_T * getCurrentLabel() {
    return current->l;
}


// Напишите спецификации к функции; также верифицируйте цикл внутри функции
/*@ 
    requires valid_file(file);
    requires valid_cmd(cmd);
    assigns \nothing;
    behavior file_is_public:
        assumes isPublicFile(file);
        ensures \result == (MAY_WRITE | MAY_READ);
    behavior cmd_gt_important:
        assumes !isPublicFile(file);
        assumes isGTImportant(cmd);
        ensures \result == MAY_WRITE;
    behavior cmd_lt_exotic:
        assumes !isPublicFile(file);
        assumes isLTExotic(cmd);
        ensures \result == MAY_READ;
    behavior cmd_equal_event:
        assumes !isPublicFile(file);
        assumes !isGTImportant(cmd);
        assumes !isLTExotic(cmd);
        assumes (!\forall int i; 0 <= i < 3 ==> event_numbers[i][0] != cmd);
        ensures \exists int j; 0 <= j < 3 && event_numbers[j][0] == cmd ==> event_numbers[j][1] == \result;
    behavior Default:
        assumes !isPublicFile(file);
        assumes !isGTImportant(cmd);
        assumes !isLTExotic(cmd);
        assumes \forall int i; 0 <= i < 3 ==> event_numbers[i][0] != cmd;
        ensures \result == (MAY_WRITE | MAY_READ);
    complete behaviors;
    disjoint behaviors;
    
*/
static int compute_mask(struct file *file, unsigned int cmd)
{
    struct inode *inode = file_inode(file);
    int mask = 0;
    int i = 0;
    // Количество событий в массиве event_numbers
    unsigned long size_array = (sizeof(event_numbers) / sizeof((event_numbers)[0]));

    // Проверяем, является ли файл публичным.
    // Если да, то доступ разрешен сразу
    if (inode->i_flags & SHIFT)
        return 0;

    if (cmd > IMPORTANT) {
        return MAY_WRITE;
    }

    if (cmd < EXOTIC) {
        return MAY_READ;
    }
    // Верифицируйте цикл внутри функции
    /*@ 
        loop invariant 0 <= i <= size_array;
        loop invariant mask == 0 || \exists int j; 0 <= j < i && cmd == event_numbers[j][0] && mask == event_numbers[j][1];
        loop assigns i, mask;
        loop variant size_array - i;
    */
    for (i = 0; i < size_array; ++i)
        if (cmd == event_numbers[i][0]) {
            mask = event_numbers[i][1];
            break;
        }

    if (!mask)
        mask = MAY_READ | MAY_WRITE;

   return mask;

}

/*@
    requires valid_file(file);
    requires valid_cmd(cmd); 
    requires \valid(current);
    requires \valid(current->l) || current->l == \null;
    requires \valid(current->process);
    assigns \nothing;
    ensures valid_result_check_permission(\result);
    behavior no_label:
        assumes getSecurityLabel(current) == \null;
        ensures \result == -NO_LABEL;
    behavior no_max_integrity_level:
        assumes getSecurityLabel(current) != \null;
		assumes !isMaxILev(current);
		ensures \result == -NO_ILEV;
    behavior is_root_can_read_and_write:
        assumes getSecurityLabel(current) != \null;
        assumes isMaxILev(current);
        assumes currentIsFileSystemRoot;
        ensures \result == 0;
    behavior is_no_root_can_read_public:
        assumes getSecurityLabel(current) != \null;
        assumes isMaxILev(current);
        assumes !currentIsFileSystemRoot;
        assumes isPublicFile(file);
        ensures \result == 0;
    behavior is_no_root_can_read_exotic:
        assumes getSecurityLabel(current) != \null;
        assumes isMaxILev(current);
        assumes !currentIsFileSystemRoot;
        assumes !isPublicFile(file);
        assumes isLTExotic(cmd);
        assumes !isGTImportant(cmd);
        ensures \result == 0;
    behavior is_no_root_cant_read_important:
        assumes getSecurityLabel(current) != \null;
        assumes isMaxILev(current);
        assumes !currentIsFileSystemRoot;
        assumes !isPublicFile(file);
        assumes !isLTExotic(cmd);
        assumes isGTImportant(cmd);
        ensures \result == -NO_PERM;
    behavior is_no_root_cant_read_cmd_event:
        assumes getSecurityLabel(current) != \null;
        assumes isMaxILev(current);
        assumes !currentIsFileSystemRoot;
        assumes !isPublicFile(file);
        assumes !isLTExotic(cmd);
        assumes !isGTImportant(cmd);
        assumes (!\forall int i; 0 <= i < 3 ==> event_numbers[i][0] != cmd);
        ensures \exists int j; 0 <= j < 3 && event_numbers[j][0] == cmd ==> \result == -NO_PERM || \result == 0;
    behavior Default:
        assumes getSecurityLabel(current) != \null;
        assumes isMaxILev(current);
        assumes !currentIsFileSystemRoot;
        assumes !isPublicFile(file);
        assumes !isLTExotic(cmd);
        assumes !isGTImportant(cmd);
        assumes \forall int i; 0 <= i < 3 ==> event_numbers[i][0] != cmd;
        ensures \result == 0;
    complete behaviors;
    disjoint behaviors;
*/
// Напишите спецификации к функции.
static int check_permission (struct file *file, unsigned int cmd)
{
    const PDPL_T *sl = getCurrentLabel();
    //@ assert sl == getSecurityLabel(current);

    // Вычисляем уровень целостности пользователя
    unsigned int ilev = slabel_ilev(sl);
    //@ assert ilev == integrityCategories(sl);

    // Если пользователь не максимального уровня целостности, возвращаем ошибку доступа
    if (ilev != max_ilev){
        return -NO_ILEV;
    }

    // Вычисляем маску 
    int mask_final = compute_mask(file, cmd);

    // Текущий процесс -- суперпользователь + у пользователя есть право на запись в файл -- доступ разрешен

    if (current->process->fsuid == 0)
        if ((mask_final & MAY_WRITE)){
            return 0;
        }

    // Текущий процесс -- не суперпользователь + у пользователя есть право на чтение из файла -- доступ разрешён

    if (!(current->process->fsuid == 0))
        if ((mask_final & MAY_READ)){
            return 0;
        }

    // В других случаях доступ запрещен
    return -NO_PERM;
}