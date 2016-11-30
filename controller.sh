function readTiva
{
  read -e input < /dev/ttyACM0;
}
up=0
down=0
left=0
right=0
while true
do
    readTiva 2> /dev/null
    case "${input:0:1}" in
        u)
        if (( down == 1 )); then
            down=0
            xdotool keyup Down
        fi
        if (( left == 1 )); then
            left=0
            xdotool keyup Left
        fi
        if (( right== 1 )); then
            right=0
            xdotool keyup Right
        fi
        if ((   up == 0 )); then
            up=1
            xdotool keydown Up
        fi;;
        d)
        if ((   up == 1 )); then
            up=0
            xdotool keyup Up
        fi
        if (( left == 1 )); then
            left=0
            xdotool keyup Left
        fi
        if (( right== 1 )); then
            right=0
            xdotool keyup Right
        fi
        if (( down == 0 )); then
            down=1
            xdotool keydown Down
        fi;;
        l)
        if (( down == 1 )); then
            down=0
            xdotool keyup Down
        fi
        if ((   up == 1 )); then
            up=0
            xdotool keyup Up
        fi
        if (( right== 1 )); then
            right=0
            xdotool keyup Right
        fi
        if (( left == 0 )); then
            left=1
            xdotool keydown Left
        fi;;
        r)
        if (( down == 1 )); then
            down=0
            xdotool keyup Down
        fi
        if (( left == 1 )); then
            left=0
            xdotool keyup Left
        fi
        if ((   up == 1 )); then
            up=0
            xdotool keyup Up
        fi
        if (( right== 0 )); then
            right=1
            xdotool keydown Right
        fi;;
        q)
        if (( down == 1 )); then
            down=0
            xdotool keyup Down
        fi
        if (( right== 1 )); then
            right=0
            xdotool keyup Right
        fi
        if (( left == 0 )); then
            left=1
            xdotool keydown Left
        fi
        if ((   up == 0 )); then
            up=1
            xdotool keydown Up
        fi;;
        e)
        if (( down == 1 )); then
            down=0
            xdotool keyup Down
        fi
        if (( left == 1 )); then
            left=0
            xdotool keyup Left
        fi
        if (( right== 0 )); then
            right=1
            xdotool keydown Right
        fi
        if ((   up == 0 )); then
            up=1
            xdotool keydown Up
        fi;;
        z)
        if ((   up == 1 )); then
            up=0
            xdotool keyup Up
        fi
        if (( right== 1 )); then
            right=0
            xdotool keyup Right
        fi
        if (( down == 0 )); then
            down=1
            xdotool keydown Down
        fi
        if (( left == 0 )); then
            left=1
            xdotool keydown Left
        fi;;
        x)
        if ((   up == 1 )); then
            up=0
            xdotool keyup Up
        fi
        if (( left == 1 )); then
            left=0
            xdotool keyup Left
        fi
        if (( right== 0 )); then
            right=1
            xdotool keydown Right
        fi
        if (( down == 0 )); then
            down=1
            xdotool keydown Down
        fi;;
        c)
        if (( down == 1 )); then
            down=0
            xdotool keyup Down
        fi
        if (( right== 1 )); then
            right=0
            xdotool keyup Right
        fi
        if (( left == 1 )); then
            left=0
            xdotool keyup Left
        fi
        if ((   up == 1 )); then
            up=0
            xdotool keyup Up
        fi;;
        p)
        xdotool keydown space;;
        o)
        xdotool keyup space;;
        s)
        xdotool keydown Return
        sleep 0.040s
        xdotool keyup Return;;
    esac
done
