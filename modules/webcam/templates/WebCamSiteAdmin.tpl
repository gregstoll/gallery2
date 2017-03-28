{*
 * $Revision: 17267 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="WebCam Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Enable"} </h3>

  <table class="gbDataTable"><tr>
    <td> {g->text text="Add From Web (http://)"} </td>
    <td>
      <select name="{g->formVar var="form[fromweb]"}">
	{html_options options=$WebCamSiteAdmin.optionList selected=$form.fromweb}
      </select>
    </td>
  </tr></table>

  <h4 class="giWarning"> {g->text text="Security Warning"} </h4>
  <ul>
    <li>
    {g->text text="The webcam module can be abused to attack other websites in your name.  For the attacked party it would seem as if you, the administrator of this Gallery, deliberately attacked their website because your Gallery acts on behalf of your users.  Therefore it is recommended to enable this feature only for trusted users."}
    </li><li>
    {g->text text='The webcam module also allows you to add files from the local server (e.g. with "file://home/youraccount/images/foo.jpg"). This particular feature is limited to site administrators since it gives access to the local file system.'}
    </li>
  </ul>
</div>

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Enter length of time a webcam item should keep one image before refreshing from the source url."}
  </p>

  <label for="duration">
    {g->text text="Duration in minutes:"}
  </label>
  <input type="text" id="duration" size="5"
   name="{g->formVar var="form[duration]"}" value="{$form.duration}"/>

  {if isset($form.error.duration)}
  <div class="giError">
    {g->text text="Invalid duration value"}
  </div>
  {/if}
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
