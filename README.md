# ittrium

**ittrium** — компактная ОС реального времени по спецификации **µITRON** (μITRON).  
Ядро — преемптивное, с приоритетными задачами; поверх него — опциональный слой платформенных сервисов (VFS, shell, lwIP, ELF).

Лицензия: [GPL-3.0](LICENSE).

---

## Возможности

### Ядро (µITRON)

| Объект | API (фрагмент) |
|--------|----------------|
| Задачи | `cre_tsk` / `act_tsk` / `ext_tsk` / `chg_pri` / `dly_tsk` / `sus_tsk` / `rsm_tsk` |
| Семафоры | `cre_sem` / `wai_sem` / `sig_sem` (+ ISR-варианты `i*`) |
| Eventflag | `cre_flg` / `set_flg` / `wai_flg` / `twai_flg` / `iset_flg` |
| Mailbox | `cre_mbx` / `snd_mbx` / `rcv_mbx` |
| Время | `set_tim` / `get_tim`, системный тик |

Контекст, диспетчер и прерывания — в порте CPU (`config/<cpu>/`).

### Сервисы (`services/`, включаются флагами `CFG_USE_*`)

| Флаг | Что даёт |
|------|----------|
| `CFG_USE_VFS` | Виртуальная ФС, mount/open/read/write |
| `CFG_USE_ROMFS` | Read-only образ в `/` |
| `CFG_USE_LFS` | littlefs (RW), обычно `/data` |
| `CFG_USE_RAMFS` | RAM FS вместо LFS |
| `CFG_USE_PROCFS` | `/proc` (tasks, interrupts, stat, …) |
| `CFG_USE_SYSFS` | `/sys` (cpu: el, midr, load, …) |
| `CFG_USE_SHELL` | Командная оболочка + line editor |
| `CFG_USE_ELF` | Загрузка/запуск статического AArch64 ELF из ФС |
| `CFG_USE_LWIP` | lwIP (`NO_SYS=0`) + shell `ifconfig` / `arp` / `ping` |

Драйверный верх (общий для плат):

- **`console`** — putc/getc для shell/логов  
- **`netdev`** — ethernet frame TX/RX для lwIP  

Низ (регистры SoC) живёт только в `example/<board>/`.

### Политика AArch64

Актуальный порт **Cortex-A53**: **EL1, без MMU** (flat physical).  
MMU / userspace / SMP — вне текущего scope.

---

## Структура репозитория

```
ittrium/
├── include/           # µITRON API (ittrium.h, service_call.h, …)
├── kernel/            # общее ядро (задачи, sem, flg, timer, …)
├── config/            # порты CPU
│   ├── cortex-a53/    # AArch64 EL1, GICv2, CNTP
│   ├── cortex-m3/ m4/
│   ├── arm7/ …
│   └── tms320c…/
├── services/          # опциональный верх (VFS, shell, net, elf, drv)
│   ├── services.mk    # подключение по CFG_USE_*
│   ├── drv/           # console, netdev
│   ├── vfs/ proc/ shell/ net/ elf/
│   └── include/
├── third_party/       # littlefs (и др.)
├── example/           # платы / демо
│   ├── qemu-a53/      # QEMU virt — основной bring-up A53
│   ├── kria-rtx/      # ZynqMP / Kria (UART1 + GEM3)
│   ├── stm32l100/ gd32f350/ c55x/ …
└── README.md
```

Вне дерева (ожидается рядом для сетевых примеров):

```
../lwip/               # lwIP upstream (services.mk → LWIPDIR=…/lwip/src)
```

---

## Порты CPU

| CPU | Каталог | Пример |
|-----|---------|--------|
| Cortex-A53 (AArch64 EL1) | `config/cortex-a53` | `example/qemu-a53`, `example/kria-rtx` |
| Cortex-M3 / M4 | `config/cortex-m3`, `cortex-m4` | `example/stm32l100`, `gd32f350` |
| ARM7 / LPC288x | `config/arm7`, `lpc288x` | — |
| M16C | `config/m16c62` | — |
| TMS320 C54x / C55x / C67x | `config/tms320c*` | `example/c55x`, `c54_test` |

Новый CPU = новый каталог в `config/` + пример в `example/`.  
Новая плата на уже портированном CPU = только `example/<board>/` (см. [портирование](#как-портировать)).

---

## Требования к сборке

- GNU Make  
- Кросс-компилятор под целевой CPU  

Для **A53** (рекомендуется Vitis baremetal):

```bash
export PATH=/opt/xilinx/Vitis/2023.2/gnu/aarch64/lin/aarch64-none/bin:$PATH
# или: export CROSS_COMPILE=aarch64-none-elf-
```

Для **qemu-a53** дополнительно:

- `qemu-system-aarch64`  
- дерево **lwIP** в `ittrium/../lwip` (если `CFG_USE_LWIP=1`)

---

## Быстрый старт: QEMU (Cortex-A53)

```bash
git clone <this-repo> ittrium
# lwIP рядом с репозиторием:
git clone https://git.savannah.nongnu.org/git/lwip.git ../lwip   # или ваш mirror

cd ittrium/example/qemu-a53
make
make run
# или из корня: ./qrun.sh
```

`make run` поднимает `virt` + GICv2 + modern virtio-net + user-net (`hostfwd` TCP `:10007` → guest `:7`).

Ожидаемый вывод: init VFS/romfs/proc/sys/lfs → virtio-net → lwIP → prompt `ittrium>`.

### Shell (qemu / kria)

```
help
ps
irq
load
ifconfig
arp
ping 10.0.2.2          # qemu user-net gateway
ls /
cat /proc/tasks
cat /proc/interrupts
cat /sys/cpu/load
run /data/app.elf
```

Подробности платы: [`example/qemu-a53/README.md`](example/qemu-a53/README.md).

---

## Плата Kria / ZynqMP

```bash
cd example/kria-rtx
make
# → ittrium-kria-rtx.elf  (load address 0x00100000)
```

- EL1, no-MMU (как QEMU)  
- Cadence **UART1** → `console`  
- Cadence **GEM3** → `netdev` + lwIP  
- GIC Group1 (EL1 NS)  
- Предполагается, что FSBL/`psu_init` уже настроил clocks/MIO/PHY  

См. [`example/kria-rtx/README.md`](example/kria-rtx/README.md).

---

## Как портировать

Два уровня: **CPU** (редко) и **плата** (часто). Ниже — типичный путь «новый SoC на уже готовом cortex-a53».

### 1. Скопировать эталон

```bash
cp -a example/qemu-a53 example/myboard
# или example/kria-rtx, если ближе ZynqMP
```

### 2. `include/target.h` — только железо

Задать MMIO/IRQ и объявления портов драйверов:

```c
#define GICD_BASE     …
#define GICC_BASE     …
#define GIC_USE_GROUP1  0   /* 1 на ZynqMP EL1 NS, 0 на QEMU virt */
#define UART_BASE     …
#define UART_IRQ      …
#define TIMER_IRQ     …     /* PPI CNTP, обычно 30 */
/* сеть: VIRTIO_NET_IRQ или GEM_IRQ — см. irq_handle в cpu_support.c */

void uart_init(void);
int  my_eth_init(void);
struct netdev *my_eth_dev(void);
```

Ядро берёт `GICD_BASE` / `GICC_BASE` / `TIMER_IRQ` из `target.h`.

### 3. `include/kernel_config.h` — объекты и сервисы

- Лимиты задач / sem / flg (ID пулы для lwIP shell и т.д.)  
- `CFG_USE_*` — что собирать через `services/services.mk`  
- Стеки и приоритеты init/shell/net  

Ненужный сервис: `#define CFG_USE_XXX 0` (или убрать define) — код не попадёт в линк.

### 4. Boot + linker

| Файл | Назначение |
|------|------------|
| `boot.S` | Спуск в EL1 (если нужно), MMU/cache off, BSS, `main` |
| `lnk.ld` | `ORIGIN` = адрес загрузки (`0x40000000` QEMU `-kernel`, `0x00100000` Kria DDR, …) |

### 5. Порт console (UART)

Реализовать `console_ops` и зарегистрировать:

```c
static const struct console_ops ops = {
  .putc = uart_putc,
  .getc = uart_getc,
};
console_register(&ops);  /* обычно в конце uart_init() */
```

Паттерн IRQ (как в qemu/kria): HW FIFO → soft FIFO → `iset_flg` → задача/`twai_flg` в getc/putc.  
Soft-vector: `install_handler(uart_irq, UART_VEC_NO, prio)` + `gic_enable_irq(UART_IRQ, prio)`.

### 6. Порт netdev (Ethernet), если нужен lwIP

```c
static const struct netdev_ops ops = {
  .init  = …,
  .start = …,
  .tx    = …,   /* сырой Ethernet frame */
  /* .set_mac — по желанию */
};
netdev_register(&dev);
netdev_init(&dev);
netdev_start(&dev);
/* на RX: dev->rx(dev, frame, len, dev->rx_arg); */
```

В `init_tsk`:

```c
net_init();
netif_netdev_add(&eth, my_eth_dev(), &ip, &mask, &gw);
netif_set_default(&eth);
```

IRQ сети мапится на soft-vec `NETDEV_VEC_NO` через `#define VIRTIO_NET_IRQ` или `#define GEM_IRQ` в `target.h` (см. `config/cortex-a53/cpu_support.c`).

### 7. Сборка примера

Как у qemu-a53:

```
example/myboard/
├── Makefile (+ TARGET)
├── Makefile.rules   # KERNEL_DIR, KERNEL_DEV=cortex-a53
├── lnk.ld, boot.S
├── include/target.h, kernel_config.h
├── kernel/Makefile  # ittrium.c + cpu_support + boot.S
└── src/
    ├── Makefile     # app SRCS + include services.mk
    └── src/         # test.c, uart.c, eth.c, syscalls.c, …
```

```bash
cd example/myboard && make
```

### 8. Smoke-checklist

1. Тик / две задачи / `dly_tsk`  
2. UART: печать + shell `help`  
3. `ps` / `irq` / `load` (telemetry)  
4. При lwIP: `ifconfig`, `ping`, TCP echo `:7`  

### Новый CPU (не только плата)

Нужен полный порт в `config/<cpu>/`:

- `cpu_support.h` / `.c` / `.S` — контекст, dispatch, IRQ stub, таймер  
- `Makefile.dev` — флаги компилятора  
- Типы и critical section под архитектуру  

Ориентир: `config/cortex-a53/` + рабочий `example/qemu-a53`.

---

## Драйверная модель (кратко)

```
shell / VFS / lwIP
        │
   console / netdev API     ← services/drv, services/net
        │
   uart.c / virtio_net.c / gem_net.c   ← только example/<board>
```

Правила:

- сервисы **не** знают `UART_BASE` / GIC SPI;  
- новый SoC = новый низ + `target.h` / `kernel_config.h`;  
- QEMU и Kria делят один верх.

---

## Telemetry

При `CFG_USE_PROCFS` / `CFG_USE_SYSFS`:

| Путь | Содержимое |
|------|------------|
| `/proc/tasks` | id, pri, state, stack used/%, cpu% |
| `/proc/interrupts` | счётчики soft-vec (timer, uart, net) |
| `/proc/stat` | агрегаты |
| `/sys/cpu/*` | el, cntfrq, midr, mmu, load |

Shell: `ps`, `irq`, `load`.

---

## Зависимости third-party

| Компонент | Где | Примечание |
|-----------|-----|------------|
| littlefs | `third_party/littlefs` | при `CFG_USE_LFS` |
| lwIP | `../lwip` (вне репо) | при `CFG_USE_LWIP`; `LWIPDIR` можно переопределить |

---

## Документация по примерам

- [`example/qemu-a53/README.md`](example/qemu-a53/README.md) — QEMU virt, virtio-net, shell  
- [`example/kria-rtx/README.md`](example/kria-rtx/README.md) — Kria / GEM3 / UART1  
- [`example/stm32l100/README.md`](example/stm32l100/README.md), [`example/gd32f350/README.md`](example/gd32f350/README.md) — Cortex-M  

---

## Что пока не в scope

- MMU / userspace / SMP на A53  
- Динамический линкер, POSIX-сигналы  
- Полноценный block-device на QSPI (littlefs сейчас часто на RAM BD)  
- Копирование Xilinx no-OS BSP (EL3+MMU) — только референс регистров  

---

## Лицензия

GNU GPL v3 — см. [LICENSE](LICENSE).
