BEGIN { getline; getline; getline; } 
{
   if ($8)
      print $2 " " $8
}
