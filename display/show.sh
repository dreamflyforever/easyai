#An infinite while loop  

i=0
while :  
do  
adb pull /oem/ws/"$i"_out.jpeg
let i++
if (("$i" >= 50))
then
i=0;
echo "i = 50"
fi
done


