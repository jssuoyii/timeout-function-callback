<?php：:q
function foo($a, $b, $c,$d){
    usleep(2*1000);
	return $a + $b + $c + $d;
}
$timeout = new Timeout();
$ret = $timeout->exec('foo', 1, 8, 2, 3, 2);
var_dump($ret,$timeout->statusCode == Timeout::TT_CALL_FUNCTION_TIMEOUT);//15  true


try{
    $ret = call_function_with_timeout('foo',1, 1,8,3,4,345);
    var_dump($ret);//16
}catch(Exception $e){
    var_dump($e->getMessage(), $e->getCode() == TT_CALL_FUNCTION_TIMEOUT);//call function timeout      true
}
?>
