<?php

include('config.php');

if (isset($_POST['submitted'])) {
  foreach($_POST as $key => $value) {
    $_POST[$key] = mysql_real_escape_string($value);
  }

  $query = "INSERT INTO bookmarks (url, title) VALUES ('" . $_POST['url'] . "','" . $_POST['title'] . "')";
  mysql_query($query) or die(mysql_error());

  $message = "The bookmark has been created successfully.";
}

?>
<?php
include_once("header.php");
?>
  <?php if (isset($message)) { ?>
  <div class="message">
    <?php echo $message; ?>
  </div>
  <?php } ?>
  <h2>Create a new bookmark</h2>
  <form action="" method="POST">
    <table>
      <tbody>
      <tr>
        <td align="right">Title:</td>
        <td><input type="text" name="title"/></td>
      </tr>
      <tr>
        <td align="right">URL:</td>
        <td><input type="text" name="url"/></td>
      </tr>
      <tr>
        <td></td>
        <td style="float: right;">
          <input type="submit" value="Save"/>
          <input type='hidden' value='1' name='submitted'/>
        </td>
      </tr>
      </tbody>
    </table>
  </form>
<?php
include_once("footer.php");
?>
