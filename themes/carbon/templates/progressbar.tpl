{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div id="ProgressBar" class="gbBlock">
  <h3 id="progressTitle">
    &nbsp;
  </h3>

  <p id="progressDescription">
    &nbsp;
  </p>

  <table width="100%" cellspacing="0" cellpadding="0">
    <tr>
      <td id="progressDone">&nbsp;</td>
      <td id="progressToGo">&nbsp;</td>
    </tr>
  </table>

  <p id="progressTimeRemaining">
    &nbsp;
  </p>

  <p id="progressMemoryInfo" style="position: absolute; top: 0px; right: 15px">
    &nbsp;
  </p>

  <p id="progressErrorInfo" style="display: none">

  </p>

  <a id="progressContinueLink" style="display: none">
    {g->text text="Continue..."}
  </a>
</div>

{*
 * After the template is sent to the browser, Gallery will send a
 * Javascript snippet to the browser that calls the updateStatus()
 * function below.  It'll call it over and over again until the task is
 * done.  The code below should update the various elements of the page
 * (title, description, etc) with the values that it receives as arguments.
 *}
{g->addToTrailer}
{literal}
<script type="text/javascript">
  // <![CDATA[
  var saveToGoDisplay = document.getElementById('progressToGo').style.display;
  function updateProgressBar(title, description, percentComplete, timeRemaining, memoryInfo) {
    document.getElementById('progressTitle').innerHTML = title;
    document.getElementById('progressDescription').innerHTML = description;

    var progressMade = Math.round(percentComplete * 100);
    var progressToGo = document.getElementById('progressToGo');

    if (progressMade == 100) {
      progressToGo.style.display = 'none';
    } else {
      progressToGo.style.display = saveToGoDisplay;
      progressToGo.style.width = (100 - progressMade) + "%";
    }

    document.getElementById('progressDone').style.width = progressMade + "%";
    document.getElementById('progressTimeRemaining').innerHTML = timeRemaining;
    document.getElementById('progressMemoryInfo').innerHTML = memoryInfo;
  }

  function completeProgressBar(url) {
    var link = document.getElementById('progressContinueLink');
    link.href = url;
    link.style.display = 'inline';
  }

  function errorProgressBar(html) {
    var errorInfo = document.getElementById('progressErrorInfo');
    errorInfo.innerHTML = html;
    errorInfo.style.display = 'block';
  }
  // ]]>
</script>
{/literal}
{/g->addToTrailer}
