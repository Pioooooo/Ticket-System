<?php
require __DIR__ . '/class/Db.php';
$result=array();
if (isset($_POST['phone']) && isset($_POST['op'])){
    $phone_regex='/^1(?:3\d{3}|5[^4\D]\d{2}|8\d{3}|7(?:[01356789]\d{2}|4(?:0\d|1[0-2]|9\d))|9[01356789]\d{2}|6[2567]\d{2})\d{6}$/';
    if (!preg_match($phone_regex,$_POST['phone'])){
        $result['e']=-1;
        $result['msg']='手机号不合法';
        echo json_encode($result);
        die();
    }
    $tmp1=$db->where('phone',$_POST['phone'])->getOne('phonecode');
    $tmp2=$db->where('phone',$_POST['phone'])->getOne('phoneuser');
    if ($_POST['op']==0 && !is_null($tmp2)){
        $result['e']=-1;
        $result['msg']='该手机号已绑定其他用户';
        echo json_encode($result);
        die();
    }
    if ($_POST['op']==1 && is_null($tmp2)){
        $result['e']=-1;
        $result['msg']='用户不存在';
        echo json_encode($result);
        die();
    }
    $code=str_pad(mt_rand(0, 999999), 6, "0", STR_PAD_BOTH);
    if (!is_null($tmp1)){
        if (time()-$tmp1['time']>=80){
            $data=array('phone'=>$_POST['phone'],
                'code'=>$code,
                'time'=>time()
            );
            $db->where('phone',$_POST['phone'])->update('phonecode',$data);
        }else{
            $result['e']=-1;
            $result['msg']='发送过于频繁，请稍后再试';
            echo json_encode($result);
            die();
        }
    }else{
        $data=array('phone'=>$_POST['phone'],
            'code'=>$code,
            'time'=>time()
        );
        $db->insert('phonecode',$data);
    }
    // send phone code
    $result['e']=0;
    $result['msg']='success';
    echo json_encode($result);
    die();
}else{
    $result['e']=-1;
    $result['msg']='error';
    echo json_encode($result);
    die();
}