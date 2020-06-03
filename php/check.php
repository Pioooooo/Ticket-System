<?php
require __DIR__ . '/class/Db.php';
$result=array();
if (isset($_POST['username'])){
    $phone_regex='/^1(?:3\d{3}|5[^4\D]\d{2}|8\d{3}|7(?:[01356789]\d{2}|4(?:0\d|1[0-2]|9\d))|9[01356789]\d{2}|6[2567]\d{2})\d{6}$/';
    if (!preg_match($phone_regex,$_POST['phone'])){
        $result['e']=-1;
        $result['msg']='phone number invalid';
        echo json_encode($result);
        die();
    }
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