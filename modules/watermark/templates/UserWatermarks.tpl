{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="My Watermarks"} </h2>
</div>

{if !empty($status) || !empty($form.error)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.add)}
    {g->text text="Watermark added successfully"}
  {/if}
  {if isset($status.delete)}
    {g->text text="Watermark deleted successfully"}
  {/if}
  {if isset($status.saved)}
    {g->text text="Watermark saved successfully"}
  {/if}
  {if isset($form.error)}
    <span class="giError">{g->text text="There was a problem processing your request."}</span>
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <h3> {g->text text="Upload Watermarks"} </h3>

  <p class="giDescription">
    {g->text text="Add your own personal watermarks here.  These watermarks can only be used by you."}
  </p>

  <table class="gbDataTable" width="100%"><tr>
    <th> {g->text text="Name"} </th>
    <th> {g->text text="Image"} </th>
    <th> {g->text text="Action"} </th>
  </tr>
  {foreach from=$UserWatermarks.watermarks item=item}
  <tr class="{cycle values="gbEven,gbOdd"}">
    <td>
      {$item.name}
    </td><td>
      {g->image item=$item image=$item maxSize=150}
    </td><td>
      <a href="{g->url arg1="view=core.UserAdmin" arg2="subView=watermark.UserWatermarkEdit"
       arg3="watermarkId=`$item.id`"}">
	{g->text text="edit"}
      </a>
      &nbsp;
      <a href="{g->url arg1="controller=watermark.UserWatermarks"
       arg2="form[action][delete]=1" arg3="form[delete][watermarkId]=`$item.id`"}">
	{g->text text="delete"}
      </a>
    </td>
  </tr>
  {/foreach}
  </table>

  <input type="file" size="60" name="{g->formVar var="form[1]"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][add]"}" value="{g->text text="Add"}"/>

  {if isset($form.error.missingFile)}
  <div class="giError">
    {g->text text="You must enter the path to a file to upload"}
  </div>
  {/if}
</div>
