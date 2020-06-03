<?php
require __DIR__ . '/class/Db.php';
$result=array();
if (isset($_POST['phone']) && isset($_POST['code'])){
    $phone_regex='/^1(?:3\d{3}|5[^4\D]\d{2}|8\d{3}|7(?:[01356789]\d{2}|4(?:0\d|1[0-2]|9\d))|9[01356789]\d{2}|6[2567]\d{2})\d{6}$/';
    if (!preg_match($phone_regex,$_POST['phone'])){
        $result['e']=-1;
        $result['msg']='phone number invalid';
        echo json_encode($result);
        die();
    }
    $tmp1=$db->where('phone',$_POST['phone'])->getOne('phonecode');
    $tmp2=$db->where('phone',$_POST['phone'])->getOne('phoneuser');
    if (!is_null($tmp1) && !is_null($tmp2)){
        if ($tmp1['code']==$_POST['code']){
            $result['e']=0;
            $result['username']=$tmp2['username'];
            echo json_encode($result);
            die();
        }else{
            $result['e']=-1;
            $result['msg']='error code';
            echo json_encode($result);
            die();
        }
    }else{
        $result['e']=-1;
        $result['msg']='phone number not exist';
        echo json_encode($result);
        die();
    }
}else{
    $result['e']=-1;
    $result['msg']='error';
    echo json_encode($result);
    die();
}