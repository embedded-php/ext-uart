--TEST--
Check if phpuart is loaded
--SKIPIF--
<?php
if (! extension_loaded('phpuart')) {
  exit('skip');
}
?>
--FILE--
<?php
echo 'The extension "phpuart" is available';
?>
--EXPECT--
The extension "phpuart" is available
