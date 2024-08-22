ENCODER_MAP_ENABLE = yes

DEFERRED_EXEC_ENABLE = yes

MIDI_ENABLE = yes

DEBOUNCE_TYPE = sym_eager_pk

AUTOCORRECT_ENABLE = yes

NKRO_ENABLE = yes

OPT_DEFS += -O2
LTO_ENABLE = yes

# STM32L432KB (128KB Flash)

#CONSOLE_ENABLE = yes
# 940 / 1350
# 960 / 1390 LTO
# 1000 / 1440 LTO + -O2
# 1060 / 1520 LTO + -O3. Unstable with RGB turned on
# 1060 / 1520 LTO + -0fast. Unstable with RGB turned on
