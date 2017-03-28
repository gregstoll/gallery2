{*
 * $Revision: 17420 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="SnapGalaxy Photo Printing Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Sell your photos as Prints &amp; Posters!  Register with %sSnapGalaxy Pro account%s and start generating revenue."
     arg1="<a href=\"http://www.snapgalaxy.com/ptinfo/ptsignup\">" arg2="</a>"}
  </p>

  <table class="gbDataTable">
    <tr><td>
      <label for="formSnapGalaxyPartnerId">
  	{g->text text="SnapGalaxy Partner ID"}
      </label>
    </td><td>
      <input type="text" size="6" id="formSnapGalaxyPartnerId" autocomplete="off"
       name="{g->formVar var="form[snapgalaxyPartnerId]"}" value="{$form.snapgalaxyPartnerId}"/>
    </td></tr>
    {if isset($form.error.snapgalaxyPartnerId.invalid)}
    <tr><td colspan="2">
      <div class="giError">
  	{g->text text="You must enter a valid snapgalaxy partner id or set it as %s if you do not have one." arg1="<b>default</b>"}
      </div>
    </td></tr>
    {/if}
  </table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
    <input type="submit" class="inputTypeSubmit"
     name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
