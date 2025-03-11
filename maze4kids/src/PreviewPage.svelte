<script lang="ts">
    import { Button } from "flowbite-svelte";
    import { type BuildMazeParams } from "./lib/maze/PaperSize";
    import { Toast } from 'flowbite-svelte';
    import { CloseCircleSolid } from 'flowbite-svelte-icons';
    import { buildMaze, BuildMazeResult } from "./lib/maze/BuildMaze";

    interface Props {
      buildParams: BuildMazeParams;
    }

    const { buildParams }: Props = $props();

    let openWindowError = $state(false);

    const result = buildMaze(buildParams);
    if (result === BuildMazeResult.ErrCannotOpenWindow) {
      openWindowError = true;
    }
</script>

<div>
  <div class="flex">
    <Button class="" onclick={() => history.back()}>
      BACK
    </Button>
    <Button class="" onclick={() => history.back()}>
      Print
    </Button>
  </div>

  {#if openWindowError}
    <Toast
      color="red"
      class="fixed top-5 right-5 border rounded-lg shadow-sm"
      on:close={() => { openWindowError = false; }}
      >
      <svelte:fragment slot="icon">
        <CloseCircleSolid class="w-5 h-5" />
        <span class="sr-only">Error icon</span>
      </svelte:fragment>
        Could not open preview window
    </Toast>
  {/if}

  PREVIEW

</div>
