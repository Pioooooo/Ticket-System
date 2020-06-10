<?php
require __DIR__ . '/class/Db.php';
$result=array();
if (isset($_POST['username'])){
    $tmp=$db->where('username',$_POST['username'])->getOne('phoneuser');
    if (!is_null($tmp)){
        $result['e']=0;
        $result['phone']=$tmp['phone'];
        echo json_encode($result);
        die();
    }else{
        $result['e']=-1;
        $result['msg']='need phone verify';
        echo json_encode($result);
        die();
    }
}else{
    $result['e']=-1;
    $result['msg']='error';
    echo json_encode($result);
    die();
}