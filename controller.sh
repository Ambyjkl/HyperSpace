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
    if [ "$input" == "gg" ]; then
        cat 4723849328437462746827348273468gdjashfgjashfuqw4873tduasgfsja.txt > /dev/ttyACM0
        break
    fi;;
done

while true
do
    readTiva 2> /dev/null
    case "${input:0:1}" in
        l)
        if (( left == 0 )); then 
            left=1
            xdotool keydown a
        fi;;
        r)
        if (( right == 0 )); then 
            right=1
            xdotool keydown d
        fi;;
        c)
        if (( right== 1 )); then
            right=0
            xdotool keyup d
        fi
        if (( left == 1 )); then
            left=0
            xdotool keyup a
        fi;;
        p)
        xdotool keydown s;;
        o)
        xdotool keyup s;;
        s)
        xdotool keydown Return
        sleep 0.040s
        xdotool keyup Return;;
    esac
done
