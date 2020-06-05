<?php
require __DIR__ . '/class/Db.php';
$result=array();
if (isset($_POST['phone']) && isset($_POST['code']) && isset($_POST['username'])){
    $phone_regex='/^1(?:3\d{3}|5[^4\D]\d{2}|8\d{3}|7(?:[01356789]\d{2}|4(?:0\d|1[0-2]|9\d))|9[01356789]\d{2}|6[2567]\d{2})\d{6}$/';
    if (!preg_match($phone_regex,$_POST['phone'])){
        $result['e']=-1;
        $result['msg']='手机号不合法';
        echo json_encode($result);
        die();
    }
    $tmp1=$db->where('phone',$_POST['phone'])->getOne('phonecode');
    $tmp2=$db->where('phone',$_POST['phone'])->getOne('phoneuser');
    if (!is_null($tmp1) && is_null($tmp2)){
        if ($tmp1['code']==$_POST['code']){
            $db->where('phone',$_POST['phone'])->delete('phonecode');
            $data=array('phone'=>$_POST['phone'],
                'username'=>$_POST['username']
            );
            $db->insert('phoneuser',$data);
            $result['e']=0;
            $result['msg']='success';
            echo json_encode($result);
            die();
        }else{
            $result['e']=-1;
            $result['msg']='验证码错误';
            echo json_encode($result);
            die();
        }
    }else{
        $result['e']=-1;
        if (is_null($tmp1)) $result['msg']='验证码错误';else $result['msg']='该手机号已绑定其他用户';
        echo json_encode($result);
        die();
    }
}else{
    $result['e']=-1;
    $result['msg']='error';
    echo json_encode($result);
    die();
}