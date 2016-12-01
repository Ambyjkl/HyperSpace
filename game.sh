function readTiva
{
    read -e input < /dev/ttyACM0;
}

function sendHighScore
{
    cat 4723849328437462746827348273468gdjashfgjashfuqw4873tduasgfsja.txt > /dev/ttyACM0
}

while true
do
    readTiva 2> /dev/null
    if [ "$input" == "gg" ]; then
        sendHighScore
        break
    fi
done

while true
do
    readTiva 2> /dev/null
    if [ "$input" == "start" ]; then
        break
    fi
done

./game & sh controller.sh
echo "wp" > /dev/ttyACM0

