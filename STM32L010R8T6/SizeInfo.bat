$Files   = Get-ChildItem -Path . -Recurse -Filter *.o -File
ForEach ($SingleFile in $Files)
  {
  arm-none-eabi-size.exe $SingleFile.fullname
  }