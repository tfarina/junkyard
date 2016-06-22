<?php

require_once 'config.php';

$debug = false;

if (isset($_POST['login'])) {
  foreach($_POST as $key => $value) {
    $data[$key] = mysql_real_escape_string($value);
  }

  $emailError = "";
  $passwordError = "";

  $valid = true;

  $email = $data['email'];
  if (empty($email)) {
    $emailError = "Please enter your email.";
    $valid = false;
  } else {
    //$email = test_input($email);
    if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
      $emailError = "Please enter a valid email address.";
      $valid = false;
    } elseif (!email_exists($email)) {
      $emailError = "Sorry, we don't recognize this email.";
      $valid = false;
    }
  }

  $plain_password = $data['password'];
  if (empty($plain_password)) {
    $passwordError = "Please enter your password.";
    $valid = false;
  } else {
    //$plain_password = test_input($plain_password);
  }

  $query = "SELECT * FROM user WHERE email='" . $email . "';";
  $result = mysql_query($query) or die(mysql_error());
  $row = mysql_fetch_assoc($result);
  if ($row) {
    $password_hash_from_db = $row['password'];
    $password_matches = password_verify($plain_password, $password_hash_from_db);
    if ($password_matches) {
      $_SESSION['userid'] = $row['user_id'];
      $_SESSION['fullname'] = $row['fullname'];
      header("location: list.php");
      exit;
    } else {
      $passwordError = "The email and password you entered don't match.";
    }
  }

  mysql_free_result($result);
}

include_once("header.php");
?>
<body style="font-family:'Helvetica Neue',Arial,Helvetica,sans-serif;background-color: #fafafa;">
<div class="container">
  <div class="row">
    <div class="panel panel-default panel-signin">
      <div class="panel-body">
        <form action="" method="post" class="form-signin form-horizontal">
          <h2 class="form-signin-heading">Log in </h2>
          <div class="form-group <?php echo !empty($emailError) ? 'has-error' : ''; ?>">
            <input type="email" class="form-control" id="user-email" name="email"
                   value="<?php echo $_POST['email']; ?>"
                   placeholder="Email" required autofocus/>
            <?php if (!empty($emailError)) { ?>
            <span class="help-block"><?php echo $emailError; ?></span>
            <?php } ?>
          </div>
          <div class="form-group <?php echo !empty($passwordError) ? 'has-error' : ''; ?>">
            <input type="password" class="form-control" id="user-password" name="password"
                   placeholder="Password" required/>
            <?php if (!empty($passwordError)) { ?>
            <span class="help-block"><?php echo $passwordError; ?></span>
            <?php } ?>
          </div>
          <div class="form-group">
            <button type="submit" name="login" class="btn btn-lg btn-block btn-primary">Log in</button>
          </div>
        </form>
      </div>
    </div>
    <p></p>
    <div class="panel panel-default panel-signup">
      <div class="panel-body">
        <p>Don't have an account? <a href="signup.php">Sign up</a></p>
      </div>
    </div>
  </div>
</div>
<?php
include_once("footer.php");
?>
