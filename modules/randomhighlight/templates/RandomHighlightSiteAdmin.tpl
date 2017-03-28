{*
 * $Revision: 16235 $
 * Read this before changing templates!  http://codex.gallery2.org/Gallery2:Editing_Templates
 *}
<div class="gbBlock gcBackground1">
  <h2> {g->text text="Random Highlight Settings"} </h2>
</div>

{if isset($status.saved)}
<div class="gbBlock"><h2 class="giSuccess">
  {g->text text="Settings saved successfully"}
</h2></div>
{/if}

<div class="gbBlock">
  <p class="giDescription">
    {g->text text="Enter length of time a random highlight album should keep one highlight before picking a new one."}
  </p>
  <p>
    <label for="duration">
      {g->text text="Duration in minutes:"}
    </label>
    <input type="text" id="duration" size="5"
     name="{g->formVar var="form[duration]"}" value="{$form.duration}"/>

    {if isset($form.error.duration)}
      <div class="giError"> {g->text text="Invalid duration value"} </div>
    {/if}
  </p>
</div>

<div class="gbBlock gcBackground1">
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][save]"}" value="{g->text text="Save"}"/>
  <input type="submit" class="inputTypeSubmit"
   name="{g->formVar var="form[action][reset]"}" value="{g->text text="Reset"}"/>
</div>
