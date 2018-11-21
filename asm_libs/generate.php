<?php
    if(count($argv) != 3 || ($argv[2] != 1 && $argv[2] != 2)){
        echo "I expect a filename and 1 if .text or 2 if .data\n";
        exit(1);
    }
    $contents = explode("\n", file_get_contents($argv[1]));
    $newfile = "/*Generated from " . $argv[1] . "*/";
    foreach($contents as $line){
        $line = str_replace("\"", "\\\"", $line);
        $line = trim($line);
        if($argv[2] == 1)
            $newfile = $newfile . "\nadd_asm(\"$line\");";
        else
            $newfile = $newfile . "\nadd_asm_values(\"$line\");";
    }
    $filename = "../lib/" . explode(".", $argv[1])[0] . ".cpp";
    file_put_contents($filename, $newfile);
?>
