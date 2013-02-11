<?php
//Hello world in php

//send content type to browser
header('Content-type: text/html; charset=utf-8');

$hello_text = 'Hello, world!';
$page_title = "$hello_text (PHP version)";
?>

<!DOCTYPE HTML>

<html>
<head>
	<title><php echo $page_title; ?></title>
</head>

<body>
	<p>
		<?php echo $hello_text; ?>
	</p>
</body>

</html>