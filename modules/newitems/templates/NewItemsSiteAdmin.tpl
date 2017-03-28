{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="New Item Settings"} </h2>
</div>

{if !empty($status)}
<div class="gbBlock"><h2 class="giSuccess">
  {if isset($status.saved)}
    {g->text text="Settings saved successfully"}
  {/if}
  {if isset($status.reset)}
    {g->text text="Album sorts reset successfully"}
  {/if}
  {if isset($status.deactivated)}
  <span class="giError">
    {g->text text="Reset albums with New Items sort to enable deactivation (see below)"}
  </span>
  {/if}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Enter length of time in days items should be marked as new and updated, or zero to disable."}
  </p>
  <table class="gbDataTable"><tr>
    <td>
      <label for="txtNew">
	{g->text text="New:"}
      </label>
    </td><td>
      <input type="text" id="txtNew" size="5"
       name="{g->formVar var="form[days][new]"}" value="{$form.days.new}"/>

      {if isset($form.error.new)}
	<div class="giError">
	  {g->text text="Invalid value"}
	</div>
      {/if}
    </td>
  </tr><tr>
    <td>
      <label for="txtUpdated">
	{g->text text="Updated:"}
      </label>
    </td><td>
      <input type="text" id="txtUpdated" size="5"
       name="{g->formVar var="form[days][updated]"}" value="{$form.days.updated}"/>

      {if isset($form.error.updated)}
	<div class="giError">
	  {g->text text="Invalid value"}
	</div>
      {/if}
    </td>
  </tr></table>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][undo]"}" value="{g->text text="Reset"}"/>
</div>

{if ($form.isDefault || $form.count > 0)}
<div class="gbBlock">
  <p class="giDescription">
    {if $form.isDefault}
      {g->text text="This gallery is using New Items sort as the default album sort.  This must be changed to another sort type before this module can be deactivated.  You can reset the default to manual sort order here."} <br/>
    {/if}
    {if ($form.count > 0)}
      {g->text one="This gallery contains %d album using New Items sort.  It must be reset to the default sort before this module can be deactivated.  You can reset the album here."
	       many="This gallery contains %d albums using New Items sort.  These must be reset to the default sort before this module can be deactivated.  You can reset all albums here."
	       count=$form.count arg1=$form.count} <br/>
    {/if}
    {g->text text="Warning: there is no undo."}
  </p>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset album sorts"}"/>
</div>
{/if}
